#include "dreamdb/executor/executor.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

#include "dreamdb/schema/database_manager.h"
#include "dreamdb/schema/database.h"
#include "dreamdb/schema/collection.h"
#include "dreamdb/schema/index_meta.h"
#include "dreamdb/schema/field.h"
#include "dreamdb/schema/entity.h"
#include "dreamdb/catalog/catalog.h"
#include "dreamdb/catalog/catalog_database_entry.h"
#include "dreamdb/catalog/catalog_collection_entry.h"
#include "dreamdb/catalog/catalog_column_entry.h"
#include "dreamdb/catalog/catalog_index_entry.h"
#include "dreamdb/catalog/catalog_vindex_entry.h"
#include "dreamdb/catalog/logical_type.h"
#include "dreamdb/common/type.h"
#include "dreamdb/expression/constant.h"
#include "dreamdb/expression/column_reference.h"
#include "dreamdb/expression/binary.h"
#include "dreamdb/evaluator/evaluator.h"
#include "dreamdb/evaluator/evaluator_context.h"
#include "dreamdb/planner/physical_planner/select/physical_seq_scan_node.h"
#include "dreamdb/planner/physical_planner/select/physical_filter_node.h"
#include "dreamdb/planner/physical_planner/select/physical_project_node.h"
#include "dreamdb/planner/physical_planner/select/physical_aggregate_node.h"
#include "dreamdb/planner/physical_planner/select/physical_sort_node.h"
#include "dreamdb/planner/physical_planner/select/physical_limit_offset_node.h"
#include "dreamdb/expression/function.h"
#include <unordered_map>

namespace dreamdb
{

namespace
{

/**
 * @brief 将 LogicalTypeId 转换为字符串
 * @param type_id 逻辑类型 ID
 * @return 类型字符串
 */
std::string logical_type_id_to_string(LogicalTypeId type_id)
{
    switch (type_id) {
        case LogicalTypeId::LOGICAL_TYPE_BOOLEAN:
            return "BOOLEAN";
        case LogicalTypeId::LOGICAL_TYPE_INTEGER:
            return "INTEGER";
        case LogicalTypeId::LOGICAL_TYPE_FLOAT:
            return "FLOAT";
        case LogicalTypeId::LOGICAL_TYPE_STRING:
            return "STRING";
        case LogicalTypeId::LOGICAL_TYPE_VECTOR:
            return "VECTOR";
        case LogicalTypeId::LOGICAL_TYPE_NULL:
            return "NULL";
        case LogicalTypeId::LOGICAL_TYPE_INVALID:
        default:
            return "INVALID";
    }
}

/**
 * @brief 将 LogicalType 转换为字符串
 * @param logical_type 逻辑类型
 * @return 类型字符串
 */
std::string logical_type_to_string(const LogicalType & logical_type)
{
    std::string type_str = logical_type_id_to_string(logical_type.id);
    if (logical_type.id == LogicalTypeId::LOGICAL_TYPE_VECTOR && logical_type.width > 0) {
        type_str += "(" + std::to_string(logical_type.width) + ")";
    }
    return type_str;
}

/**
 * @brief 将 FieldType 转换为 LogicalTypeId
 * @param field_type 字段类型
 * @return 逻辑类型 ID
 */
LogicalTypeId field_type_to_logical_type_id(FieldType field_type)
{
    switch (field_type) {
        case FieldType::BOOLEAN:
            return LogicalTypeId::LOGICAL_TYPE_BOOLEAN;
        case FieldType::TINYINT:
        case FieldType::SMALLINT:
        case FieldType::INTEGER:
        case FieldType::BIGINT:
        case FieldType::TIMESTAMP:
            return LogicalTypeId::LOGICAL_TYPE_INTEGER;
        case FieldType::FLOAT:
        case FieldType::DOUBLE:
        case FieldType::DECIMAL:
            return LogicalTypeId::LOGICAL_TYPE_FLOAT;
        case FieldType::CHAR:
        case FieldType::VARCHAR:
        case FieldType::ENUM:
            return LogicalTypeId::LOGICAL_TYPE_STRING;
        case FieldType::VECTOR:
            return LogicalTypeId::LOGICAL_TYPE_VECTOR;
        default:
            return LogicalTypeId::LOGICAL_TYPE_INVALID;
    }
}

/**
 * @brief 将 Field 转换为 LogicalType
 * @param field 字段定义
 * @return 逻辑类型
 */
LogicalType field_to_logical_type(const Field & field)
{
    LogicalType logical_type;
    logical_type.id = field_type_to_logical_type_id(field.get_type());
    logical_type.nullable = field.get_is_nullable();
    
    // 对于 VECTOR 类型，使用 length 作为 width（维度）
    if (field.get_type() == FieldType::VECTOR) {
        logical_type.width = static_cast<std::size_t>(field.get_length());
    }
    else {
        logical_type.width = 0;
    }
    
    return logical_type;
}

/**
 * @brief 评估表达式获取字段值（用于 INSERT）
 * @param expr 表达式
 * @return 字段值，如果评估失败返回 std::nullopt
 */
std::optional<FieldValue> evaluate_expression_for_insert(const Expression * expr)
{
    if (!expr) {
        return std::nullopt;
    }
    
    // 对于常量表达式，直接获取值
    if (expr->get_type() == ExpressionType::EXPRESSION_CONSTANT) {
        const auto * const_expr = static_cast<const ConstantExpression *>(expr);
        return const_expr->get_field_value();
    }
    
    // 对于其他表达式，需要使用 Evaluator
    // 但 INSERT 语句中的值应该是常量，所以这里简化处理
    // 如果遇到非常量表达式，返回错误
    return std::nullopt;
}

/**
 * @brief 查询结果行（用于 SELECT）
 */
struct QueryRow
{
    std::vector<FieldValue> values;  // 行的值列表
    std::size_t entity_id = 0;       // 实体 ID（用于 UPDATE/DELETE）
};

/**
 * @brief 评估表达式获取字段值（用于 Filter 和 Project）
 * @param expr 表达式
 * @param row 当前行的值
 * @return 字段值，如果评估失败返回 std::nullopt
 */
std::optional<FieldValue> evaluate_expression_for_row(
    const Expression * expr,
    const QueryRow & row
)
{
    if (!expr) {
        return std::nullopt;
    }
    
    switch (expr->get_type()) {
        case ExpressionType::EXPRESSION_CONSTANT: {
            const auto * const_expr = static_cast<const ConstantExpression *>(expr);
            return const_expr->get_field_value();
        }
        case ExpressionType::EXPRESSION_COLUMN_REFERENCE: {
            const auto * col_expr = static_cast<const ColumnReferenceExpression *>(expr);
            std::size_t field_index = col_expr->get_field_index();
            if (field_index < row.values.size()) {
                return row.values[field_index];
            }
            return std::nullopt;
        }
        default:
            // 其他表达式类型（如二元表达式）在 evaluate_condition_for_row 中处理
            return std::nullopt;
    }
}

/**
 * @brief 简单比较两个 FieldValue（简化实现）
 * @param left 左值
 * @param right 右值
 * @return 比较结果：-1 (left < right), 0 (left == right), 1 (left > right)，如果类型不兼容返回 std::nullopt
 */
std::optional<int> simple_compare_values(const FieldValue & left, const FieldValue & right)
{
    // 简化实现：只支持相同类型的比较
    if (left.index() != right.index()) {
        return std::nullopt;
    }
    
    // 使用 variant 的索引来判断类型并比较
    switch (left.index()) {
        case 0: { // std::int8_t
            return std::get<std::int8_t>(left) < std::get<std::int8_t>(right) ? -1 :
                   std::get<std::int8_t>(left) > std::get<std::int8_t>(right) ? 1 : 0;
        }
        case 1: { // std::int16_t
            return std::get<std::int16_t>(left) < std::get<std::int16_t>(right) ? -1 :
                   std::get<std::int16_t>(left) > std::get<std::int16_t>(right) ? 1 : 0;
        }
        case 2: { // std::int32_t
            return std::get<std::int32_t>(left) < std::get<std::int32_t>(right) ? -1 :
                   std::get<std::int32_t>(left) > std::get<std::int32_t>(right) ? 1 : 0;
        }
        case 3: { // std::int64_t
            return std::get<std::int64_t>(left) < std::get<std::int64_t>(right) ? -1 :
                   std::get<std::int64_t>(left) > std::get<std::int64_t>(right) ? 1 : 0;
        }
        case 4: { // float
            float l = std::get<float>(left);
            float r = std::get<float>(right);
            return l < r ? -1 : l > r ? 1 : 0;
        }
        case 5: { // double
            double l = std::get<double>(left);
            double r = std::get<double>(right);
            return l < r ? -1 : l > r ? 1 : 0;
        }
        case 7: { // std::string
            const std::string & l = std::get<std::string>(left);
            const std::string & r = std::get<std::string>(right);
            return l < r ? -1 : l > r ? 1 : 0;
        }
        case 8: { // bool
            bool l = std::get<bool>(left);
            bool r = std::get<bool>(right);
            return l < r ? -1 : l > r ? 1 : 0;
        }
        default:
            // 其他类型（Decimal, VECTOR, Null）暂不支持比较
            return std::nullopt;
    }
}

/**
 * @brief 评估条件表达式（用于 Filter）
 * @param expr 表达式
 * @param row 当前行的值
 * @param evaluator 评估器（用于比较值）
 * @return 布尔值，如果评估失败返回 std::nullopt
 */
std::optional<bool> evaluate_condition_for_row(
    const Expression * expr,
    const QueryRow & row,
    const Evaluator & evaluator
)
{
    if (!expr) {
        return std::nullopt;
    }
    
    // 对于二元表达式，尝试评估为布尔值
    if (expr->get_type() == ExpressionType::EXPRESSION_BINARY) {
        const auto * bin_expr = static_cast<const BinaryExpression *>(expr);
        BinaryOperatorType op = bin_expr->get_operator_type();
        
        auto left_val = evaluate_expression_for_row(&bin_expr->get_left(), row);
        auto right_val = evaluate_expression_for_row(&bin_expr->get_right(), row);
        
        if (!left_val.has_value() || !right_val.has_value()) {
            return std::nullopt;
        }
        
        // 使用简单比较函数
        auto compare_result = simple_compare_values(left_val.value(), right_val.value());
        
        if (!compare_result.has_value()) {
            return std::nullopt;
        }
        
        int cmp = compare_result.value();
        
        switch (op) {
            case BinaryOperatorType::EXPRESSION_EQUAL:
                return (cmp == 0);
            case BinaryOperatorType::EXPRESSION_NOT_EQUAL:
                return (cmp != 0);
            case BinaryOperatorType::EXPRESSION_GREATER_THAN:
                return (cmp > 0);
            case BinaryOperatorType::EXPRESSION_GREATER_EQUAL:
                return (cmp >= 0);
            case BinaryOperatorType::EXPRESSION_LESS_THAN:
                return (cmp < 0);
            case BinaryOperatorType::EXPRESSION_LESS_EQUAL:
                return (cmp <= 0);
            case BinaryOperatorType::EXPRESSION_AND: {
                // 对于 AND，递归评估两个操作数
                auto left_bool = evaluate_condition_for_row(&bin_expr->get_left(), row, evaluator);
                auto right_bool = evaluate_condition_for_row(&bin_expr->get_right(), row, evaluator);
                if (left_bool.has_value() && right_bool.has_value()) {
                    return left_bool.value() && right_bool.value();
                }
                return std::nullopt;
            }
            case BinaryOperatorType::EXPRESSION_OR: {
                // 对于 OR，递归评估两个操作数
                auto left_bool = evaluate_condition_for_row(&bin_expr->get_left(), row, evaluator);
                auto right_bool = evaluate_condition_for_row(&bin_expr->get_right(), row, evaluator);
                if (left_bool.has_value() && right_bool.has_value()) {
                    return left_bool.value() || right_bool.value();
                }
                return std::nullopt;
            }
            default:
                return std::nullopt;
        }
    }
    
    return std::nullopt;
}

// 前向声明
std::vector<QueryRow> execute_physical_plan_node(
    const PhysicalPlanNode * plan_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
);

std::vector<QueryRow> execute_select_plan_node(
    const PhysicalSelectPlanNode * select_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
);

std::vector<QueryRow> execute_seq_scan(
    const PhysicalSeqScanNode * scan_node,
    DatabaseManager * database_manager
);

std::vector<QueryRow> execute_filter(
    const PhysicalFilterNode * filter_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
);

std::vector<QueryRow> execute_project(
    const PhysicalProjectNode * project_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
);

std::vector<QueryRow> execute_aggregate(
    const PhysicalAggregateNode * aggregate_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
);

std::vector<QueryRow> execute_sort(
    const PhysicalSortNode * sort_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
);

std::vector<QueryRow> execute_limit_offset(
    const PhysicalLimitOffsetNode * limit_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
);

/**
 * @brief 格式化表格输出
 * @param column_name 列名
 * @param rows 行数据
 * @return 格式化后的字符串
 */
std::string format_table(const std::string & column_name, const std::vector<std::string> & rows)
{
    if (rows.empty()) {
        // 空表
        std::size_t width = std::max(column_name.size(), static_cast<std::size_t>(8));
        std::ostringstream oss;
        oss << "+" << std::string(width + 2, '-') << "+\n";
        oss << "| " << std::setw(static_cast<int>(width)) << std::left << column_name << " |\n";
        oss << "+" << std::string(width + 2, '-') << "+\n";
        return oss.str();
    }

    // 计算列宽（列名和所有行的最大宽度）
    std::size_t width = column_name.size();
    for (const auto & row : rows) {
        width = std::max(width, row.size());
    }
    // 最小宽度为 8
    width = std::max(width, static_cast<std::size_t>(8));

    std::ostringstream oss;
    
    // 顶部边框
    oss << "+" << std::string(width + 2, '-') << "+\n";
    
    // 表头
    oss << "| " << std::setw(static_cast<int>(width)) << std::left << column_name << " |\n";
    
    // 分隔线
    oss << "+" << std::string(width + 2, '-') << "+\n";
    
    // 数据行
    for (const auto & row : rows) {
        oss << "| " << std::setw(static_cast<int>(width)) << std::left << row << " |\n";
    }
    
    // 底部边框
    oss << "+" << std::string(width + 2, '-') << "+\n";
    
    return oss.str();
}

/**
 * @brief 执行物理计划节点并返回查询结果（实现）
 */
std::vector<QueryRow> execute_physical_plan_node(
    const PhysicalPlanNode * plan_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
)
{
    if (!plan_node) {
        return {};
    }
    
    // 根据操作类型分发
    switch (plan_node->get_operation_type()) {
        case PhysicalPlanNodeOperationType::PHYSICAL_PLAN_SELECT: {
            const auto * select_node = static_cast<const PhysicalSelectPlanNode *>(plan_node);
            return execute_select_plan_node(select_node, database_manager, evaluator);
        }
        default:
            return {};
    }
}

/**
 * @brief 执行 SELECT 物理计划节点
 */
std::vector<QueryRow> execute_select_plan_node(
    const PhysicalSelectPlanNode * select_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
)
{
    if (!select_node) {
        return {};
    }
    
    switch (select_node->get_operator_type()) {
        case PhysicalSelectOperatorType::SELECT_SEQ_SCAN: {
            const auto * scan_node = static_cast<const PhysicalSeqScanNode *>(select_node);
            return execute_seq_scan(scan_node, database_manager);
        }
        case PhysicalSelectOperatorType::SELECT_FILTER: {
            const auto * filter_node = static_cast<const PhysicalFilterNode *>(select_node);
            return execute_filter(filter_node, database_manager, evaluator);
        }
        case PhysicalSelectOperatorType::SELECT_PROJECT: {
            const auto * project_node = static_cast<const PhysicalProjectNode *>(select_node);
            return execute_project(project_node, database_manager, evaluator);
        }
        case PhysicalSelectOperatorType::SELECT_AGGREGATE: {
            const auto * aggregate_node = static_cast<const PhysicalAggregateNode *>(select_node);
            return execute_aggregate(aggregate_node, database_manager, evaluator);
        }
        case PhysicalSelectOperatorType::SELECT_SORT: {
            const auto * sort_node = static_cast<const PhysicalSortNode *>(select_node);
            return execute_sort(sort_node, database_manager, evaluator);
        }
        case PhysicalSelectOperatorType::SELECT_LIMIT_OFFSET: {
            const auto * limit_node = static_cast<const PhysicalLimitOffsetNode *>(select_node);
            return execute_limit_offset(limit_node, database_manager, evaluator);
        }
        default:
            return {};
    }
}

/**
 * @brief 执行全表扫描
 */
std::vector<QueryRow> execute_seq_scan(
    const PhysicalSeqScanNode * scan_node,
    DatabaseManager * database_manager
)
{
    if (!scan_node || !database_manager) {
        return {};
    }
    
    // 查找集合
    Catalog & catalog = database_manager->get_catalog();
    std::size_t collection_id = scan_node->get_collection_id();
    
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return {};
    }
    
    // 获取集合对象
    Database * database = database_manager->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return {};
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return {};
    }
    
    // 获取所有实体（全表扫描）
    std::vector<std::unique_ptr<Entity>> entities = collection->get_all_entities();
    
    // 转换为 QueryRow
    std::vector<QueryRow> rows;
    const std::vector<std::size_t> & field_indexes = scan_node->get_field_indexes();
    
    for (const auto & entity : entities) {
        QueryRow row;
        if (field_indexes.empty()) {
            // 如果没有指定字段，返回所有字段
            for (std::size_t i = 0; i < entity->field_count(); ++i) {
                row.values.push_back(entity->get_value(i));
            }
        } else {
            // 返回指定字段
            for (std::size_t field_index : field_indexes) {
                if (field_index < entity->field_count()) {
                    row.values.push_back(entity->get_value(field_index));
                }
            }
        }
        rows.push_back(std::move(row));
    }
    
    return rows;
}

/**
 * @brief 执行过滤操作
 */
std::vector<QueryRow> execute_filter(
    const PhysicalFilterNode * filter_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
)
{
    if (!filter_node || !database_manager) {
        return {};
    }
    
    // 执行子节点（应该是扫描节点）
    const auto & children = filter_node->get_children();
    if (children.empty()) {
        return {};
    }
    
    std::vector<QueryRow> input_rows = execute_physical_plan_node(
        static_cast<const PhysicalPlanNode *>(children[0].get()),
        database_manager,
        evaluator
    );
    
    // 获取集合信息用于评估表达式
    // 这里简化处理，假设子节点是扫描节点
    Collection * collection = nullptr;
    // TODO: 从子节点获取集合信息用于表达式评估
    // Catalog & catalog = database_manager->get_catalog();
    // Database * database = database_manager->get_current_database();
    
    if (!collection) {
        // 如果无法获取集合，跳过过滤（简化处理）
        return input_rows;
    }
    
    // 评估谓词并过滤
    std::vector<QueryRow> filtered_rows;
    const Expression & predicate = filter_node->get_predicate();
    
    for (const auto & row : input_rows) {
        // 评估条件表达式
        auto condition_result = evaluate_condition_for_row(&predicate, row, evaluator);
        
        // 如果条件为 true，保留该行
        if (condition_result.has_value() && condition_result.value()) {
            filtered_rows.push_back(row);
        }
    }
    
    return filtered_rows;
}

/**
 * @brief 执行投影操作
 */
std::vector<QueryRow> execute_project(
    const PhysicalProjectNode * project_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
)
{
    if (!project_node || !database_manager) {
        return {};
    }
    
    // 执行子节点
    const auto & children = project_node->get_children();
    if (children.empty()) {
        return {};
    }
    
    std::vector<QueryRow> input_rows = execute_physical_plan_node(
        static_cast<const PhysicalPlanNode *>(children[0].get()),
        database_manager,
        evaluator
    );
    
    // 执行投影
    std::vector<QueryRow> projected_rows;
    const auto & project_items = project_node->get_project_items();
    
    for (const auto & input_row : input_rows) {
        QueryRow projected_row;
        
        // 评估每个投影表达式
        for (const auto & item : project_items) {
            auto value = evaluate_expression_for_row(item.expression.get(), input_row);
            if (value.has_value()) {
                projected_row.values.push_back(value.value());
            } else {
                // 如果评估失败，添加 NULL 值
                projected_row.values.push_back(Null{});
            }
        }
        
        // 保留实体 ID（用于后续操作）
        projected_row.entity_id = input_row.entity_id;
        
        projected_rows.push_back(std::move(projected_row));
    }
    
    return projected_rows;
}

/**
 * @brief 执行聚合操作
 */
std::vector<QueryRow> execute_aggregate(
    const PhysicalAggregateNode * aggregate_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
)
{
    if (!aggregate_node || !database_manager) {
        return {};
    }
    
    // 执行子节点
    const auto & children = aggregate_node->get_children();
    if (children.empty()) {
        return {};
    }
    
    std::vector<QueryRow> input_rows = execute_physical_plan_node(
        static_cast<const PhysicalPlanNode *>(children[0].get()),
        database_manager,
        evaluator
    );
    
    // TODO: 实现聚合逻辑
    // 简化处理：返回空结果
    return {};
}

/**
 * @brief 执行排序操作
 */
std::vector<QueryRow> execute_sort(
    const PhysicalSortNode * sort_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
)
{
    if (!sort_node || !database_manager) {
        return {};
    }
    
    // 执行子节点
    const auto & children = sort_node->get_children();
    if (children.empty()) {
        return {};
    }
    
    std::vector<QueryRow> input_rows = execute_physical_plan_node(
        static_cast<const PhysicalPlanNode *>(children[0].get()),
        database_manager,
        evaluator
    );
    
    // TODO: 实现排序逻辑
    // 简化处理：返回输入行（不排序）
    return input_rows;
}

/**
 * @brief 执行限制和偏移操作
 */
std::vector<QueryRow> execute_limit_offset(
    const PhysicalLimitOffsetNode * limit_node,
    DatabaseManager * database_manager,
    const Evaluator & evaluator
)
{
    if (!limit_node || !database_manager) {
        return {};
    }
    
    // 执行子节点
    const auto & children = limit_node->get_children();
    if (children.empty()) {
        return {};
    }
    
    std::vector<QueryRow> input_rows = execute_physical_plan_node(
        static_cast<const PhysicalPlanNode *>(children[0].get()),
        database_manager,
        evaluator
    );
    
    // 应用偏移
    std::size_t offset = limit_node->get_offset().value_or(0);
    if (offset >= input_rows.size()) {
        return {};
    }
    
    std::vector<QueryRow> result(input_rows.begin() + offset, input_rows.end());
    
    // 应用限制
    if (limit_node->get_limit().has_value()) {
        std::size_t limit = limit_node->get_limit().value();
        if (limit < result.size()) {
            result.resize(limit);
        }
    }
    
    return result;
}

} // anonymous namespace

Executor::Executor(std::unique_ptr<DatabaseManager> database_manager)
    : database_manager_(std::move(database_manager))
{
}

const Catalog & Executor::get_catalog() const noexcept
{
    return database_manager_->get_catalog();
}

Catalog & Executor::get_catalog() noexcept
{
    return database_manager_->get_catalog();
}

std::string Executor::get_current_database_name() const
{
    Database * current_db = database_manager_->get_current_database();
    if (current_db) {
        return current_db->get_name();
    }
    return "";
}

MutationResult Executor::execute(const PhysicalPlanNode & physical_plan)
{
    switch (physical_plan.get_operation_type())
    {
        case PhysicalPlanNodeOperationType::PHYSICAL_PLAN_SELECT:
            return execute_select(static_cast<const PhysicalSelectPlanNode &>(physical_plan));
        case PhysicalPlanNodeOperationType::PHYSICAL_PLAN_UPDATE:
            return execute_update(static_cast<const PhysicalUpdatePlanNode &>(physical_plan));
        case PhysicalPlanNodeOperationType::PHYSICAL_PLAN_DELETE:
            return execute_delete(static_cast<const PhysicalDeletePlanNode &>(physical_plan));
        default:
            return MutationResult::make_failure("Unsupported physical plan operation type");
    }
}

MutationResult Executor::execute(const BoundStatement & bound_statement)
{
    switch (bound_statement.get_type()) {
        case BoundStatementType::BINDER_BOUND_USE_STATEMENT:
            return execute_use(static_cast<const BoundUseStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_CREATE_STATEMENT:
            return execute_create(static_cast<const BoundCreateStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_DROP_STATEMENT:
            return execute_drop(static_cast<const BoundDropStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_ALTER_STATEMENT:
            return execute_alter(static_cast<const BoundAlterStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_DESCRIBE_STATEMENT:
            return execute_describe(static_cast<const BoundDescribeStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_SHOW_STATEMENT:
            return execute_show(static_cast<const BoundShowStatement &>(bound_statement));
        case BoundStatementType::BINDER_BOUND_INSERT_STATEMENT:
            return execute_insert(static_cast<const BoundInsertStatement &>(bound_statement));
        default:
            return MutationResult::make_failure("Unsupported bound statement type");
    }
}

MutationResult Executor::execute_select(const PhysicalSelectPlanNode & select_plan)
{
    Evaluator evaluator;
    const PhysicalPlanNode * plan_node = static_cast<const PhysicalPlanNode *>(&select_plan);
    std::vector<QueryRow> rows = execute_physical_plan_node(plan_node, database_manager_.get(), evaluator);
    
    MutationResult result = MutationResult::make_success();
    result.set_affected_count(rows.size());
    result.set_message(std::to_string(rows.size()) + " row(s) returned");
    
    // TODO: 格式化输出查询结果（可以后续实现）
    
    return result;
}

MutationResult Executor::execute_update(const PhysicalUpdatePlanNode & update_plan)
{
    // 执行 SELECT 子计划获取要更新的行 ID
    Evaluator evaluator;
    const PhysicalPlanNode * select_plan = update_plan.get_select_plan();
    if (!select_plan) {
        return MutationResult::make_failure("UPDATE plan missing SELECT sub-plan");
    }
    
    // 执行 SELECT 子计划获取实体列表
    // 注意：这里简化处理，假设 SELECT 返回的是实体 ID 列表
    // 实际应该从 SELECT 结果中提取实体 ID
    std::vector<QueryRow> rows = execute_physical_plan_node(select_plan, database_manager_.get(), evaluator);
    
    // 查找集合
    Catalog & catalog = database_manager_->get_catalog();
    std::size_t collection_id = update_plan.get_collection_id();
    
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 获取集合对象
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // TODO: 从 SELECT 结果中提取实体 ID，然后对每个实体执行更新
    // 这里简化处理，暂时返回未实现
    return MutationResult::make_failure("UPDATE execution not fully implemented yet");
}

MutationResult Executor::execute_delete(const PhysicalDeletePlanNode & delete_plan)
{
    // 执行 SELECT 子计划获取要删除的行 ID
    Evaluator evaluator;
    const PhysicalPlanNode * select_plan = delete_plan.get_select_plan();
    if (!select_plan) {
        return MutationResult::make_failure("DELETE plan missing SELECT sub-plan");
    }
    
    // 执行 SELECT 子计划获取实体列表
    std::vector<QueryRow> rows = execute_physical_plan_node(select_plan, database_manager_.get(), evaluator);
    
    // 查找集合
    Catalog & catalog = database_manager_->get_catalog();
    std::size_t collection_id = delete_plan.get_collection_id();
    
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 获取集合对象
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // TODO: 从 SELECT 结果中提取实体 ID，然后对每个实体执行删除
    // 这里简化处理，暂时返回未实现
    return MutationResult::make_failure("DELETE execution not fully implemented yet");
}

MutationResult Executor::execute_use(const BoundUseStatement & use_statement)
{
    database_manager_->set_current_database(use_statement.database_id);
    MutationResult result = MutationResult::make_success();
    result.set_message("Database changed.");
    return result;
}

MutationResult Executor::execute_create(const BoundCreateStatement & create_statement)
{
    if (std::holds_alternative<std::monostate>(create_statement.create_operation)) {
        return MutationResult::make_failure("Invalid CREATE operation");
    }

    if (std::holds_alternative<BoundCreateDatabase>(create_statement.create_operation)) {
        const auto & op = std::get<BoundCreateDatabase>(create_statement.create_operation);
        return execute_create_database(op.database_name, create_statement.if_not_exists);
    }
    else if (std::holds_alternative<BoundCreateCollection>(create_statement.create_operation)) {
        const auto & op = std::get<BoundCreateCollection>(create_statement.create_operation);
        return execute_create_collection(op.collection_name, op.column_definitions, create_statement.if_not_exists);
    }
    else if (std::holds_alternative<BoundCreateIndex>(create_statement.create_operation)) {
        const auto & op = std::get<BoundCreateIndex>(create_statement.create_operation);
        return execute_create_index(op.collection_id, op.index_name, op.column_ids, op.index_type, create_statement.if_not_exists);
    }
    else if (std::holds_alternative<BoundCreateVIndex>(create_statement.create_operation)) {
        const auto & op = std::get<BoundCreateVIndex>(create_statement.create_operation);
        return execute_create_vindex(op.collection_id, op.vindex_name, op.column_id, op.vindex_type, op.with_clauses, create_statement.if_not_exists);
    }

    return MutationResult::make_failure("Unsupported CREATE operation");
}

MutationResult Executor::execute_drop(const BoundDropStatement & drop_statement)
{
    if (std::holds_alternative<std::monostate>(drop_statement.drop_operation)) {
        return MutationResult::make_failure("Invalid DROP operation");
    }

    if (std::holds_alternative<BoundDropDatabase>(drop_statement.drop_operation)) {
        const auto & op = std::get<BoundDropDatabase>(drop_statement.drop_operation);
        return execute_drop_database(op.database_id, drop_statement.if_exists);
    }
    else if (std::holds_alternative<BoundDropCollection>(drop_statement.drop_operation)) {
        const auto & op = std::get<BoundDropCollection>(drop_statement.drop_operation);
        return execute_drop_collection(op.collection_id, drop_statement.if_exists);
    }
    else if (std::holds_alternative<BoundDropIndex>(drop_statement.drop_operation)) {
        const auto & op = std::get<BoundDropIndex>(drop_statement.drop_operation);
        return execute_drop_index(op.collection_id, op.index_name, drop_statement.if_exists);
    }
    else if (std::holds_alternative<BoundDropVIndex>(drop_statement.drop_operation)) {
        const auto & op = std::get<BoundDropVIndex>(drop_statement.drop_operation);
        return execute_drop_vindex(op.collection_id, op.vindex_name, drop_statement.if_exists);
    }

    return MutationResult::make_failure("Unsupported DROP operation");
}

MutationResult Executor::execute_alter(const BoundAlterStatement & alter_statement)
{
    if (std::holds_alternative<std::monostate>(alter_statement.alter_operation)) {
        return MutationResult::make_failure("Invalid ALTER operation");
    }

    if (std::holds_alternative<BoundAlterAddColumn>(alter_statement.alter_operation)) {
        const auto & op = std::get<BoundAlterAddColumn>(alter_statement.alter_operation);
        return execute_alter_add_column(alter_statement.collection_id, op.column_definition);
    }
    else if (std::holds_alternative<BoundAlterDropColumn>(alter_statement.alter_operation)) {
        const auto & op = std::get<BoundAlterDropColumn>(alter_statement.alter_operation);
        return execute_alter_drop_column(alter_statement.collection_id, op.column_id);
    }
    else if (std::holds_alternative<BoundAlterModifyColumn>(alter_statement.alter_operation)) {
        const auto & op = std::get<BoundAlterModifyColumn>(alter_statement.alter_operation);
        return execute_alter_modify_column(alter_statement.collection_id, op.column_id, op.new_definition);
    }
    else if (std::holds_alternative<BoundAlterRenameColumn>(alter_statement.alter_operation)) {
        const auto & op = std::get<BoundAlterRenameColumn>(alter_statement.alter_operation);
        return execute_alter_rename_column(alter_statement.collection_id, op.column_id, op.new_name);
    }

    return MutationResult::make_failure("Unsupported ALTER operation");
}

MutationResult Executor::execute_describe(const BoundDescribeStatement & describe_statement)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的集合条目
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == describe_statement.collection_id) {
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 获取所有列信息
    std::vector<std::string> column_names = collection_entry->get_column_names();
    
    // 计算各列的宽度
    std::size_t field_width = std::max(static_cast<std::size_t>(5), std::string("Field").size());
    std::size_t type_width = std::max(static_cast<std::size_t>(4), std::string("Type").size());
    std::size_t null_width = std::max(static_cast<std::size_t>(4), std::string("Null").size());
    
    // 收集所有列的数据
    struct ColumnInfo
    {
        std::string field;
        std::string type;
        std::string null;
    };
    
    std::vector<ColumnInfo> columns;
    columns.reserve(column_names.size());
    
    for (const auto & column_name : column_names) {
        const CatalogColumnEntry * column_entry = collection_entry->get_column_entry(column_name);
        if (!column_entry) {
            continue;
        }
        
        ColumnInfo info;
        info.field = column_name;
        info.type = logical_type_to_string(column_entry->logical_type());
        info.null = column_entry->logical_type().nullable ? "YES" : "NO";
        
        field_width = std::max(field_width, info.field.size());
        type_width = std::max(type_width, info.type.size());
        null_width = std::max(null_width, info.null.size());
        
        columns.push_back(std::move(info));
    }
    
    // 格式化输出
    std::ostringstream oss;
    
    // 顶部边框
    oss << "+" << std::string(field_width + 2, '-') 
        << "+" << std::string(type_width + 2, '-')
        << "+" << std::string(null_width + 2, '-') << "+\n";
    
    // 表头
    oss << "| " << std::setw(static_cast<int>(field_width)) << std::left << "Field"
        << " | " << std::setw(static_cast<int>(type_width)) << std::left << "Type"
        << " | " << std::setw(static_cast<int>(null_width)) << std::left << "Null"
        << " |\n";
    
    // 分隔线
    oss << "+" << std::string(field_width + 2, '-') 
        << "+" << std::string(type_width + 2, '-')
        << "+" << std::string(null_width + 2, '-') << "+\n";
    
    // 数据行
    for (const auto & col : columns) {
        oss << "| " << std::setw(static_cast<int>(field_width)) << std::left << col.field
            << " | " << std::setw(static_cast<int>(type_width)) << std::left << col.type
            << " | " << std::setw(static_cast<int>(null_width)) << std::left << col.null
            << " |\n";
    }
    
    // 底部边框
    oss << "+" << std::string(field_width + 2, '-') 
        << "+" << std::string(type_width + 2, '-')
        << "+" << std::string(null_width + 2, '-') << "+\n";
    
    MutationResult result = MutationResult::make_success();
    result.set_message(oss.str());
    return result;
}

MutationResult Executor::execute_show(const BoundShowStatement & show_statement)
{
    if (std::holds_alternative<std::monostate>(show_statement.show_operation)) {
        return MutationResult::make_failure("Invalid SHOW operation");
    }

    if (std::holds_alternative<BoundShowDatabases>(show_statement.show_operation)) {
        return execute_show_databases();
    }
    else if (std::holds_alternative<BoundShowCollections>(show_statement.show_operation)) {
        const auto & op = std::get<BoundShowCollections>(show_statement.show_operation);
        return execute_show_collections(op.database_id);
    }
    else if (std::holds_alternative<BoundShowIndexes>(show_statement.show_operation)) {
        const auto & op = std::get<BoundShowIndexes>(show_statement.show_operation);
        return execute_show_indexes(op.collection_id, op.database_id);
    }
    else if (std::holds_alternative<BoundShowVIndexes>(show_statement.show_operation)) {
        const auto & op = std::get<BoundShowVIndexes>(show_statement.show_operation);
        return execute_show_vindexes(op.collection_id, op.database_id);
    }

    return MutationResult::make_failure("Unsupported SHOW operation");
}

MutationResult Executor::execute_show_databases()
{
    // 获取数据库名称列表
    const Catalog & catalog = database_manager_->get_catalog();
    std::vector<std::string> database_names = catalog.get_database_names();
    
    // 格式化输出
    std::string output = format_table("Database", database_names);
    
    MutationResult result = MutationResult::make_success();
    result.set_message(output);
    return result;
}

MutationResult Executor::execute_show_collections(std::optional<std::size_t> database_id)
{
    const Catalog & catalog = database_manager_->get_catalog();
    std::vector<std::string> collection_names;
    
    if (database_id.has_value()) {
        // 从指定的数据库获取集合列表
        const CatalogDatabaseEntry * database_entry = catalog.get_database_entry(database_id.value());
        if (!database_entry) {
            return MutationResult::make_failure("Database not found");
        }
        collection_names = database_entry->get_collection_names();
    }
    else {
        // 从当前数据库获取集合列表
        Database * current_database = database_manager_->get_current_database();
        if (!current_database) {
            return MutationResult::make_failure("No database selected");
        }

        // 从 Catalog 获取当前数据库的条目
        const CatalogDatabaseEntry * database_entry = catalog.get_database_entry(current_database->get_name());
        if (!database_entry) {
            return MutationResult::make_failure("Current database not found in catalog");
        }
        collection_names = database_entry->get_collection_names();
    }

    // 格式化输出
    std::string output = format_table("Collection", collection_names);

    MutationResult result = MutationResult::make_success();
    result.set_message(output);
    return result;
}

MutationResult Executor::execute_show_indexes(std::size_t /*collection_id*/, std::optional<std::size_t> /*database_id*/)
{
    // TODO: 实现 SHOW INDEXES 执行逻辑
    return MutationResult::make_failure("SHOW INDEXES execution not implemented yet");
}

MutationResult Executor::execute_show_vindexes(std::size_t /*collection_id*/, std::optional<std::size_t> /*database_id*/)
{
    // TODO: 实现 SHOW VINDEXES 执行逻辑
    return MutationResult::make_failure("SHOW VINDEXES execution not implemented yet");
}

MutationResult Executor::execute_drop_database(std::size_t database_id, bool if_exists)
{
    // 检查数据库是否存在
    if (!database_manager_->has_database(database_id)) {
        if (if_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Database does not exist, skipping.");
            return result;
        }
        return MutationResult::make_failure("Database not found");
    }

    // 删除数据库
    bool success = database_manager_->drop_database(database_id);
    if (!success) {
        return MutationResult::make_failure("Failed to drop database");
    }

    MutationResult result = MutationResult::make_success();
    result.set_message("Database dropped successfully.");
    return result;
}

MutationResult Executor::execute_drop_collection(std::size_t collection_id, bool if_exists)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        if (if_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Collection does not exist, skipping.");
            return result;
        }
        return MutationResult::make_failure("Collection not found");
    }
    
    // 从 Database 删除集合
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        // 如果当前数据库不匹配，需要找到对应的数据库
        // 这里简化处理，假设集合在当前数据库中
        return MutationResult::make_failure("Collection not in current database");
    }
    
    bool success = database->drop_collection(collection_entry->collection_name_);
    if (!success) {
        return MutationResult::make_failure("Failed to drop collection");
    }
    
    // 从 Catalog 删除集合条目
    CatalogDatabaseEntry * mutable_db_entry = const_cast<CatalogDatabaseEntry *>(database_entry);
    mutable_db_entry->remove_collection(collection_entry->collection_name_);
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Collection dropped successfully.");
    return result;
}

MutationResult Executor::execute_drop_index(std::size_t collection_id, const std::string & index_name, bool if_exists)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库和集合
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        if (if_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Collection does not exist, skipping.");
            return result;
        }
        return MutationResult::make_failure("Collection not found");
    }
    
    // 检查索引是否存在
    if (!collection_entry->get_index_entry(index_name)) {
        if (if_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Index does not exist, skipping.");
            return result;
        }
        return MutationResult::make_failure("Index not found");
    }
    
    // 从 Database 删除索引
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    bool success = collection->remove_index(index_name);
    if (!success) {
        return MutationResult::make_failure("Failed to drop index");
    }
    
    // 从 Catalog 删除索引条目
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    mutable_coll_entry->remove_index(index_name);
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Index dropped successfully.");
    return result;
}

MutationResult Executor::execute_drop_vindex(std::size_t collection_id, const std::string & vindex_name, bool if_exists)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的集合条目
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        if (if_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Collection does not exist, skipping.");
            return result;
        }
        return MutationResult::make_failure("Collection not found");
    }
    
    // 检查向量索引是否存在
    if (!collection_entry->get_vindex_entry(vindex_name)) {
        if (if_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Vector index does not exist, skipping.");
            return result;
        }
        return MutationResult::make_failure("Vector index not found");
    }
    
    // 从 Catalog 删除向量索引条目
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    bool success = mutable_coll_entry->remove_vindex(vindex_name);
    if (!success) {
        return MutationResult::make_failure("Failed to drop vector index");
    }
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Vector index dropped successfully.");
    return result;
}

MutationResult Executor::execute_create_database(const std::string & database_name, bool if_not_exists)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 检查数据库是否已存在
    if (catalog.has_database(database_name)) {
        if (if_not_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Database already exists, skipping.");
            return result;
        }
        return MutationResult::make_failure("Database already exists");
    }
    
    // 创建数据库
    std::size_t database_id = database_manager_->create_database(database_name);
    if (database_id == 0) {
        return MutationResult::make_failure("Failed to create database");
    }
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Database created successfully.");
    return result;
}

MutationResult Executor::execute_create_collection(
    const std::string & collection_name,
    const std::vector<Field> & column_definitions,
    bool if_not_exists
)
{
    // 获取当前数据库
    Database * current_database = database_manager_->get_current_database();
    if (!current_database) {
        return MutationResult::make_failure("No database selected");
    }
    
    // 检查集合是否已存在
    if (current_database->has_collection(collection_name)) {
        if (if_not_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Collection already exists, skipping.");
            return result;
        }
        return MutationResult::make_failure("Collection already exists");
    }
    
    // 创建集合
    Collection * collection = current_database->create_collection(collection_name, column_definitions);
    if (!collection) {
        return MutationResult::make_failure("Failed to create collection");
    }
    
    // 在 Catalog 中创建集合条目
    Catalog & catalog = database_manager_->get_catalog();
    const CatalogDatabaseEntry * database_entry = catalog.get_database_entry(current_database->get_name());
    if (!database_entry) {
        return MutationResult::make_failure("Current database not found in catalog");
    }
    
    // 创建集合条目
    auto collection_entry = std::make_unique<CatalogCollectionEntry>(collection_name);
    
    // 添加列条目
    for (std::size_t i = 0; i < column_definitions.size(); ++i) {
        const Field & field = column_definitions[i];
        LogicalType logical_type = field_to_logical_type(field);
        auto column_entry = std::make_unique<CatalogColumnEntry>(field.get_name(), logical_type, i);
        collection_entry->add_column(std::move(column_entry));
    }
    
    // 将集合条目添加到数据库条目
    CatalogDatabaseEntry * mutable_db_entry = const_cast<CatalogDatabaseEntry *>(database_entry);
    mutable_db_entry->add_collection(std::move(collection_entry));
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Collection created successfully.");
    return result;
}

MutationResult Executor::execute_create_index(
    std::size_t collection_id,
    const std::string & index_name,
    const std::vector<std::size_t> & column_ids,
    IndexType index_type,
    bool if_not_exists
)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库和集合
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 检查索引是否已存在
    if (collection_entry->get_index_entry(index_name)) {
        if (if_not_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Index already exists, skipping.");
            return result;
        }
        return MutationResult::make_failure("Index already exists");
    }
    
    // 从 Database 获取集合并创建索引
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // 创建 IndexMeta
    IndexMeta index_meta;
    index_meta.set_index_name(index_name);
    index_meta.set_index_type(index_type);
    index_meta.set_is_unique(false);  // 默认非唯一索引，可以根据需要调整
    for (std::size_t column_id : column_ids) {
        index_meta.add_field_index(column_id);
    }
    
    // 在 Collection 中创建索引
    bool success = collection->create_index(index_meta);
    if (!success) {
        return MutationResult::make_failure("Failed to create index in collection");
    }
    
    // 在 Catalog 中创建索引条目
    auto catalog_index_entry = std::make_unique<CatalogIndexEntry>(
        index_name,
        index_type,
        false,  // 默认非唯一
        column_ids
    );
    
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    mutable_coll_entry->add_index(std::move(catalog_index_entry));
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Index created successfully.");
    return result;
}

MutationResult Executor::execute_create_vindex(
    std::size_t collection_id,
    const std::string & vindex_name,
    std::size_t column_id,
    VIndexType vindex_type,
    const std::vector<std::pair<std::string, std::string>> & with_clauses,
    bool if_not_exists
)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的集合条目
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 检查向量索引是否已存在
    if (collection_entry->get_vindex_entry(vindex_name)) {
        if (if_not_exists) {
            MutationResult result = MutationResult::make_success();
            result.set_message("Vector index already exists, skipping.");
            return result;
        }
        return MutationResult::make_failure("Vector index already exists");
    }
    
    // 检查列是否存在
    const CatalogColumnEntry * column_entry = collection_entry->get_column_entry(column_id);
    if (!column_entry) {
        return MutationResult::make_failure("Column not found");
    }
    
    // 检查列类型是否为 VECTOR
    if (column_entry->logical_type().id != LogicalTypeId::LOGICAL_TYPE_VECTOR) {
        return MutationResult::make_failure("Column is not a vector type");
    }
    
    // 解析 WITH 子句中的 metric_type（如果有）
    std::optional<MetricType> metric_type = std::nullopt;
    for (const auto & [key, value] : with_clauses) {
        if (key == "metric_type" || key == "metric") {
            if (value == "L2" || value == "l2") {
                metric_type = MetricType::L2;
            }
            else if (value == "IP" || value == "ip") {
                metric_type = MetricType::IP;
            }
            else if (value == "COSINE" || value == "cosine") {
                metric_type = MetricType::COSINE;
            }
        }
    }
    
    // 在 Catalog 中创建向量索引条目
    auto catalog_vindex_entry = std::make_unique<CatalogVIndexEntry>(
        vindex_name,
        column_id,
        vindex_type,
        metric_type
    );
    
    // 设置其他配置选项
    for (const auto & [key, value] : with_clauses) {
        if (key != "metric_type" && key != "metric") {
            catalog_vindex_entry->set_option(key, value);
        }
    }
    
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    mutable_coll_entry->add_vindex(std::move(catalog_vindex_entry));
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Vector index created successfully.");
    return result;
}

MutationResult Executor::execute_alter_add_column(std::size_t collection_id, const Field & column_definition)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库和集合
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 检查列是否已存在
    if (collection_entry->has_column(column_definition.get_name())) {
        return MutationResult::make_failure("Column already exists");
    }
    
    // 从 Database 获取集合
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // TODO: 在 Collection 中添加列（需要 Collection 提供 add_column 方法）
    // 目前先只在 Catalog 中添加
    
    // 在 Catalog 中添加列条目
    std::size_t new_column_index = collection_entry->get_column_names().size();
    LogicalType logical_type = field_to_logical_type(column_definition);
    auto column_entry = std::make_unique<CatalogColumnEntry>(
        column_definition.get_name(),
        logical_type,
        new_column_index
    );
    
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    mutable_coll_entry->add_column(std::move(column_entry));
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Column added successfully.");
    return result;
}

MutationResult Executor::execute_alter_drop_column(std::size_t collection_id, std::size_t column_id)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库和集合
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 获取列条目
    const CatalogColumnEntry * column_entry = collection_entry->get_column_entry(column_id);
    if (!column_entry) {
        return MutationResult::make_failure("Column not found");
    }
    
    // 从 Database 获取集合
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // TODO: 在 Collection 中删除列（需要 Collection 提供 remove_column 方法）
    // 目前先只在 Catalog 中删除
    
    // 在 Catalog 中删除列条目
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    bool success = mutable_coll_entry->remove_column(column_entry->name());
    if (!success) {
        return MutationResult::make_failure("Failed to remove column from catalog");
    }
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Column dropped successfully.");
    return result;
}

MutationResult Executor::execute_alter_modify_column(
    std::size_t collection_id,
    std::size_t column_id,
    const Field & new_definition
)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库和集合
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 获取列条目
    const CatalogColumnEntry * column_entry = collection_entry->get_column_entry(column_id);
    if (!column_entry) {
        return MutationResult::make_failure("Column not found");
    }
    
    // 从 Database 获取集合
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // TODO: 在 Collection 中修改列（需要 Collection 提供 modify_column 方法）
    // 目前先只在 Catalog 中修改
    
    // 创建新的列条目
    LogicalType logical_type = field_to_logical_type(new_definition);
    CatalogColumnEntry new_column_entry(new_definition.get_name(), logical_type, column_id);
    
    // 在 Catalog 中修改列条目
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    bool success = mutable_coll_entry->modify_column(column_entry->name(), new_column_entry);
    if (!success) {
        return MutationResult::make_failure("Failed to modify column in catalog");
    }
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Column modified successfully.");
    return result;
}

MutationResult Executor::execute_alter_rename_column(
    std::size_t collection_id,
    std::size_t column_id,
    const std::string & new_name
)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库和集合
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 获取列条目
    const CatalogColumnEntry * column_entry = collection_entry->get_column_entry(column_id);
    if (!column_entry) {
        return MutationResult::make_failure("Column not found");
    }
    
    // 检查新名称是否已存在
    if (collection_entry->has_column(new_name)) {
        return MutationResult::make_failure("Column name already exists");
    }
    
    // 从 Database 获取集合
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // TODO: 在 Collection 中重命名列（需要 Collection 提供 rename_column 方法）
    // 目前先只在 Catalog 中重命名
    
    // 在 Catalog 中重命名列条目
    // 注意：CatalogColumnEntry 没有直接的 rename 方法，需要先删除再添加
    // 或者使用 modify_column 方法（如果支持重命名）
    // 这里简化处理，使用 modify_column 并保持其他属性不变
    CatalogColumnEntry new_column_entry(new_name, column_entry->logical_type(), column_id);
    
    CatalogCollectionEntry * mutable_coll_entry = const_cast<CatalogCollectionEntry *>(collection_entry);
    bool success = mutable_coll_entry->modify_column(column_entry->name(), new_column_entry);
    if (!success) {
        return MutationResult::make_failure("Failed to rename column in catalog");
    }
    
    MutationResult result = MutationResult::make_success();
    result.set_message("Column renamed successfully.");
    return result;
}

MutationResult Executor::execute_insert(const BoundInsertStatement & insert_statement)
{
    Catalog & catalog = database_manager_->get_catalog();
    
    // 查找包含该集合的数据库和集合
    const CatalogDatabaseEntry * database_entry = nullptr;
    const CatalogCollectionEntry * collection_entry = nullptr;
    
    // 遍历所有数据库查找集合
    std::vector<std::string> database_names = catalog.get_database_names();
    for (const auto & database_name : database_names) {
        const CatalogDatabaseEntry * db_entry = catalog.get_database_entry(database_name);
        if (!db_entry) {
            continue;
        }
        
        // 遍历该数据库的所有集合
        std::vector<std::string> collection_names = db_entry->get_collection_names();
        for (const auto & collection_name : collection_names) {
            const CatalogCollectionEntry * coll_entry = db_entry->get_collection_entry(collection_name);
            if (coll_entry && coll_entry->collection_id_ == insert_statement.collection_id) {
                database_entry = db_entry;
                collection_entry = coll_entry;
                break;
            }
        }
        
        if (collection_entry) {
            break;
        }
    }
    
    if (!collection_entry) {
        return MutationResult::make_failure("Collection not found");
    }
    
    // 从 Database 获取集合
    Database * database = database_manager_->get_current_database();
    if (!database || database->get_name() != database_entry->database_name_) {
        // 如果当前数据库不匹配，需要找到对应的数据库
        // 这里简化处理，假设集合在当前数据库中
        return MutationResult::make_failure("Collection not in current database");
    }
    
    Collection * collection = database->get_collection(collection_entry->collection_name_);
    if (!collection) {
        return MutationResult::make_failure("Collection not found in database");
    }
    
    // 创建实体
    Entity entity = collection->create_entity();
    
    // 获取集合的 schema 信息
    const std::vector<Field> & schema = collection->get_schema();
    
    // 评估并设置每个插入项的值
    for (const auto & item : insert_statement.insert_items) {
        if (item.column_index >= schema.size()) {
            return MutationResult::make_failure("Column index out of range");
        }
        
        // 评估表达式获取字段值
        std::optional<FieldValue> value = evaluate_expression_for_insert(item.value.get());
        
        if (!value.has_value()) {
            return MutationResult::make_failure("Failed to evaluate expression for column " + std::to_string(item.column_index));
        }
        
        // 检查类型兼容性（简化处理，这里假设类型已经由 Binder 验证）
        // 设置实体字段值
        entity.set_value(item.column_index, value.value());
    }
    
    // 对于未指定的字段，如果字段允许 NULL，则设置为 NULL
    // 如果字段不允许 NULL 且没有默认值，则返回错误
    for (std::size_t i = 0; i < schema.size(); ++i) {
        // 检查该字段是否在 insert_items 中
        bool found = false;
        for (const auto & item : insert_statement.insert_items) {
            if (item.column_index == i) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            const Field & field = schema[i];
            if (!field.get_is_nullable()) {
                return MutationResult::make_failure("Column " + std::to_string(i) + " is not nullable and no value provided");
            }
            // 对于可空字段，如果未指定值，可以保持默认值（如果有）或设置为 NULL
            // 这里简化处理，不设置值（Entity 创建时可能已有默认值）
        }
    }
    
    // 插入实体
    MutationResult result = collection->insert(entity);
    
    if (result.is_success()) {
        result.set_message("1 row inserted");
        result.set_affected_count(1);
    }
    
    return result;
}

} // namespace dreamdb
