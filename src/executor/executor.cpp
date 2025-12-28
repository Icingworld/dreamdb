#include "dreamdb/executor/executor.h"

#include <algorithm>

#include "dreamdb/parser/ast/select_stmt.h"
#include "dreamdb/parser/ast/delete_stmt.h"
#include "dreamdb/parser/ast/insert_stmt.h"
#include "dreamdb/parser/ast/update_stmt.h"
#include "dreamdb/parser/ast/create_stmt.h"
#include "dreamdb/parser/ast/drop_stmt.h"
#include "dreamdb/parser/ast/use_stmt.h"
#include "dreamdb/parser/ast/describe_stmt.h"
#include "dreamdb/parser/ast/show_stmt.h"
#include "dreamdb/parser/ast/alter_stmt.h"
#include "dreamdb/schema/database.h"
#include "dreamdb/schema/collection.h"
#include "dreamdb/parser/ast/literal_expr.h"
#include "dreamdb/query/query.h"
#include "dreamdb/query/order.h"
#include "dreamdb/query/limit.h"

namespace dreamdb
{

ExecutorResult::ExecutorResult()
    : is_success_(false)
    , message_()
    , affected_count_(0)
    , rows_()
{
}

void ExecutorResult::set_is_success(bool is_success) noexcept
{
    is_success_ = is_success;
}

void ExecutorResult::set_message(const std::string & message)
{
    message_ = message;
}

void ExecutorResult::set_affected_count(std::size_t affected_count) noexcept
{
    affected_count_ = affected_count;
}

void ExecutorResult::add_row(Entity && entity)
{
    rows_->emplace_back(std::move(entity));
}

bool ExecutorResult::get_is_success() const noexcept
{
    return is_success_;
}

const std::string & ExecutorResult::get_message() const noexcept
{
    return message_;
}

std::size_t ExecutorResult::get_affected_count() const noexcept
{
    return affected_count_.value();
}

std::size_t ExecutorResult::get_row_count() const noexcept
{
    return rows_->size();
}

const std::vector<Entity> & ExecutorResult::get_rows() const noexcept
{
    return rows_.value();
}

Executor::Executor(std::unique_ptr<DatabaseManager> database_manager)
    : database_manager_(std::move(database_manager))
{
}

Executor::~Executor() = default;

ExecutorResult Executor::execute(const AstNode & ast)
{
    // 根据不同语句类型执行不同操作
    switch (ast.get_type()) {
        case AstNodeType::SELECT_STMT:
            return execute_select(static_cast<const SelectStmt &>(ast));
        case AstNodeType::DELETE_STMT:
            return execute_delete(static_cast<const DeleteStmt &>(ast));
        case AstNodeType::INSERT_STMT:
            return execute_insert(static_cast<const InsertStmt &>(ast));
        case AstNodeType::UPDATE_STMT:
            return execute_update(static_cast<const UpdateStmt &>(ast));
        case AstNodeType::CREATE_STMT:
            return execute_create(static_cast<const CreateStmt &>(ast));
        case AstNodeType::DROP_STMT:
            return execute_drop(static_cast<const DropStmt &>(ast));
        case AstNodeType::USE_STMT:
            return execute_use(static_cast<const UseStmt &>(ast));
        case AstNodeType::DESCRIBE_STMT:
            return execute_describe(static_cast<const DescribeStmt &>(ast));
        case AstNodeType::SHOW_STMT:
            return execute_show(static_cast<const ShowStmt &>(ast));
        case AstNodeType::ALTER_STMT:
            return execute_alter(static_cast<const AlterStmt &>(ast));
        default: {
            ExecutorResult result;
            result.set_is_success(false);
            result.set_message("Unsupported AST node type: " + ast.debug_string());
            return result;
        }
    }
}

ExecutorResult Executor::execute_select(const SelectStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_select not implemented");
    return result;
}

ExecutorResult Executor::execute_delete(const DeleteStmt & delete_stmt)
{
    // 获取删除的集合名称
    const std::string & collection_name = delete_stmt.get_collection_name();

    ExecutorResult result;

    // 获取当前数据库
    Database * database = get_current_database();
    if (database == nullptr) {
        result.set_is_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 获取集合
    Collection * collection = database->get_collection(collection_name);
    if (collection == nullptr) {
        result.set_is_success(false);
        result.set_message("Unknown collection: '" + collection_name + "'");
        return result;
    }

    // 构造一个 Query
    Query query;
    // 设置 where 条件
    const AstNode * where_clause = delete_stmt.get_where_clause();
    if (where_clause != nullptr) {
        query.set_where_clause(where_clause);
    }
    // 设置 order by 条件
    std::optional<std::string> order_column = delete_stmt.get_order_column();
    if (order_column.has_value()) {
        // 存在排序，设置排序条件
        std::optional<std::size_t> field_index = collection->get_field_index(order_column.value());
        if (field_index.has_value()) {
            // Order 需要 std::uint8_t，进行类型转换
            if (field_index.value() > 255) {
                result.set_is_success(false);
                result.set_message("Field index too large for ordering");
                return result;
            }
            // 获取 order_type，也需要检查是否存在
            std::optional<Direction> order_type_opt = delete_stmt.get_order_type();
            Direction order_type = order_type_opt.value_or(Direction::ASC); // 默认 ASC
            query.set_order(Order(static_cast<std::uint8_t>(field_index.value()), order_type));
        } else {
            result.set_is_success(false);
            result.set_message("Unknown column: '" + order_column.value() + "'");
            return result;
        }
    }
    // 设置 limit 条件
    std::optional<std::size_t> limit = delete_stmt.get_limit();
    if (limit.has_value()) {
        query.set_limit(Limit(limit.value()));
    }
    
    // 执行查询，获取符合条件的实体
    std::vector<std::unique_ptr<Entity>> entities = collection->query(query);
    
    // 删除查询结果中的实体
    std::size_t deleted_count = 0;
    for (const auto & entity : entities) {
        MutationResult remove_result = collection->remove(entity->get_id());
        if (remove_result.is_success()) {
            deleted_count++;
        }
    }
    
    result.set_is_success(true);
    result.set_affected_count(deleted_count);
    result.set_message("Deleted " + std::to_string(deleted_count) + " row(s)");
    return result;
}

ExecutorResult Executor::execute_insert(const InsertStmt & insert_stmt)
{
    // 获取插入的集合名称
    const std::string & collection_name = insert_stmt.get_collection_name();

    // 获取插入的列名
    const std::vector<std::string> & column_names = insert_stmt.get_column_names();
    // 获取插入的值
    const std::vector<std::unique_ptr<AstNode>> & values = insert_stmt.get_values();

    ExecutorResult result;

    // 获取当前数据库
    Database * database = get_current_database();
    if (database == nullptr) {
        result.set_is_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 获取集合
    Collection * collection = database->get_collection(collection_name);
    if (collection == nullptr) {
        result.set_is_success(false);
        result.set_message("Unknown collection: '" + collection_name + "'");
        return result;
    }

    // 获取集合的 Schema
    const std::vector<Field> & schema = collection->get_schema();

    // 检查值和列名的数量匹配
    if (!column_names.empty() && column_names.size() != values.size()) {
        result.set_is_success(false);
        result.set_message("Number of columns and values do not match");
        return result;
    }

    // 创建实体
    Entity entity = collection->create_entity();
    // 记录是否提供值
    std::vector<bool> is_filled(schema.size(), false);

    if (column_names.empty()) {
        // 按表结构顺序插入
        if (values.size() != schema.size()) {
            result.set_is_success(false);
            result.set_message("Number of values does not match number of columns");
            return result;
        }
        for (std::size_t i = 0; i < values.size(); ++i) {
            // 转换 AstNode 为 FieldValue
            FieldType field_type = schema[i].get_type();
            FieldValue field_value = ast_to_field_value(values[i].get(), field_type);
            
            is_filled[i] = true;
            entity.set_value(i, field_value);
        }
    } else {
        // 填充已有的列名和值
        for (std::size_t i = 0; i < column_names.size(); ++i) {
            const std::string & column_name = column_names[i];
            const std::unique_ptr<AstNode> & value = values[i];
            
            // 找到该列的索引
            std::optional<std::size_t> index = collection->get_field_index(column_name);
            if (!index.has_value()) {
                result.set_is_success(false);
                result.set_message("Unknown column: '" + column_name + "'");
                return result;
            }
            
            // 检查重复列名
            if (is_filled[index.value()]) {
                result.set_is_success(false);
                result.set_message("Duplicate column: '" + column_name + "'");
                return result;
            }
            
            // 转换 AstNode 为 FieldValue
            FieldType field_type = schema[index.value()].get_type();
            FieldValue field_value = ast_to_field_value(value.get(), field_type);

            is_filled[index.value()] = true;
            entity.set_value(index.value(), field_value);
        }
    }

    // 统一验证实体合法性
    for (std::size_t i = 0; i < schema.size(); ++i) {
        if (is_filled[i]) {
            // 已填充，验证值的合法性
            const FieldValue & value = entity.get_value(i);

            // 验证 NULL 约束
            if (std::holds_alternative<Null>(value) && !schema[i].get_is_nullable()) {
                result.set_is_success(false);
                result.set_message("Field '" + schema[i].get_name() + "' cannot be NULL");
                return result;
            }

            // 验证 ENUM 约束（仅在非 NULL 时验证）
            if (schema[i].get_type() == FieldType::ENUM && !std::holds_alternative<Null>(value)) {
                if (!std::holds_alternative<std::string>(value)) {
                    result.set_is_success(false);
                    result.set_message("Field '" + schema[i].get_name() + "' must be a string");
                    return result;
                }
                const std::vector<std::string> & options = schema[i].get_options();
                const std::string & str_value = std::get<std::string>(value);
                if (std::find(options.begin(), options.end(), str_value) == options.end()) {
                    result.set_is_success(false);
                    result.set_message("Invalid value for field '" + schema[i].get_name() + "': '" + str_value + "'");
                    return result;
                }
            }

            // 验证字符串长度约束
            if ((schema[i].get_type() == FieldType::VARCHAR || schema[i].get_type() == FieldType::CHAR)
                && !std::holds_alternative<Null>(value)) {
                if (!std::holds_alternative<std::string>(value)) {
                    result.set_is_success(false);
                    result.set_message("Field '" + schema[i].get_name() + "' must be a string");
                    return result;
                }
                const std::string & str_value = std::get<std::string>(value);
                if (static_cast<int>(str_value.length()) > schema[i].get_length()) {
                    result.set_is_success(false);
                    result.set_message("Field '" + schema[i].get_name() + "' exceeds maximum length " 
                                     + std::to_string(schema[i].get_length()));
                    return result;
                }
            }

            // 验证 PRIMARY KEY 约束
            // TODO: 实现 PRIMARY KEY 约束验证
        } else {
            // 未填充，验证默认值、自动递增、NULL 检查

            // 验证自动递增
            if (schema[i].get_is_auto_increment()) {
                // TODO: 实现自动递增
                continue;
            }

            // 允许 NULL，验证默认值
            const FieldValue & default_value = schema[i].get_default_value();
            // 当前实现中，不设置默认值时，默认值为 Null()
            if (!std::holds_alternative<Null>(default_value)) {
                // 有默认值，使用默认值，无论是否允许 NULL
                entity.set_value(i, default_value);
                continue;
            }

            // 验证 NULL 约束
            if (!schema[i].get_is_nullable()) {
                // 不允许 NULL，且没有默认值，报错
                result.set_is_success(false);
                result.set_message("Field '" + schema[i].get_name() + "' is required but not provided");
                return result;
            }

            // 允许 NULL，且没有默认值，设置为 NULL
            // Entity 构造函数已初始化为 Null()，无需设置
            // entity.set_value(i, Null());
        }
    }

    // 验证合法，执行插入
    MutationResult mutation_result = collection->insert(entity);
    if (!mutation_result.is_success()) {
        result.set_is_success(false);
        result.set_message(mutation_result.get_error_message());
        return result;
    }

    result.set_is_success(true);
    result.set_message("Inserted " + std::to_string(mutation_result.get_affected_count()) + " rows");
    result.set_affected_count(1);
    return result;
}

ExecutorResult Executor::execute_update(const UpdateStmt & update_stmt)
{
    // 获取更新的集合名称
    const std::string & collection_name = update_stmt.get_collection_name();

    ExecutorResult result;

    // 获取当前数据库
    Database * database = get_current_database();
    if (database == nullptr) {
        result.set_is_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 获取集合
    Collection * collection = database->get_collection(collection_name);
    if (collection == nullptr) {
        result.set_is_success(false);
        result.set_message("Unknown collection: '" + collection_name + "'");
        return result;
    }

    // 获取集合的 Schema
    const std::vector<Field> & schema = collection->get_schema();

    // 获取 SET 子句的赋值列表
    const std::vector<std::pair<std::string, std::unique_ptr<AstNode>>> & assignments = update_stmt.get_assignments();
    if (assignments.empty()) {
        result.set_is_success(false);
        result.set_message("No fields to update");
        return result;
    }

    // 验证所有列名是否存在，并构建更新字段列表（字段索引 -> 字段值）
    std::vector<std::pair<std::size_t, FieldValue>> update_fields;
    for (const auto & assignment : assignments) {
        const std::string & column_name = assignment.first;
        const std::unique_ptr<AstNode> & value_expr = assignment.second;

        // 查找列索引
        std::optional<std::size_t> field_index = collection->get_field_index(column_name);
        if (!field_index.has_value()) {
            result.set_is_success(false);
            result.set_message("Unknown column: '" + column_name + "'");
            return result;
        }

        // 检查重复列名（在同一个 UPDATE 语句中）
        for (const auto & [idx, _] : update_fields) {
            if (idx == field_index.value()) {
                result.set_is_success(false);
                result.set_message("Duplicate column: '" + column_name + "'");
                return result;
            }
        }

        // 转换 AstNode 为 FieldValue
        FieldType field_type = schema[field_index.value()].get_type();
        FieldValue field_value = ast_to_field_value(value_expr.get(), field_type);

        // 验证值的合法性
        // 验证 NULL 约束
        if (std::holds_alternative<Null>(field_value) && !schema[field_index.value()].get_is_nullable()) {
            result.set_is_success(false);
            result.set_message("Field '" + column_name + "' cannot be NULL");
            return result;
        }

        // 验证 ENUM 约束
        if (schema[field_index.value()].get_type() == FieldType::ENUM && !std::holds_alternative<Null>(field_value)) {
            if (!std::holds_alternative<std::string>(field_value)) {
                result.set_is_success(false);
                result.set_message("Field '" + column_name + "' must be a string");
                return result;
            }
            const std::vector<std::string> & options = schema[field_index.value()].get_options();
            const std::string & str_value = std::get<std::string>(field_value);
            if (std::find(options.begin(), options.end(), str_value) == options.end()) {
                result.set_is_success(false);
                result.set_message("Invalid value for field '" + column_name + "': '" + str_value + "'");
                return result;
            }
        }

        // 验证字符串长度约束
        if ((schema[field_index.value()].get_type() == FieldType::VARCHAR || 
             schema[field_index.value()].get_type() == FieldType::CHAR) &&
            !std::holds_alternative<Null>(field_value)) {
            if (!std::holds_alternative<std::string>(field_value)) {
                result.set_is_success(false);
                result.set_message("Field '" + column_name + "' must be a string");
                return result;
            }
            const std::string & str_value = std::get<std::string>(field_value);
            if (static_cast<int>(str_value.length()) > schema[field_index.value()].get_length()) {
                result.set_is_success(false);
                result.set_message("Field '" + column_name + "' exceeds maximum length " 
                                 + std::to_string(schema[field_index.value()].get_length()));
                return result;
            }
        }

        update_fields.emplace_back(field_index.value(), field_value);
    }

    // 构造一个 Query
    Query query;
    // 设置 where 条件
    const AstNode * where_clause = update_stmt.get_where_clause();
    if (where_clause != nullptr) {
        query.set_where_clause(where_clause);
    }
    // 设置 order by 条件
    const std::optional<std::string> & order_column = update_stmt.get_order_column();
    if (order_column.has_value()) {
        // 存在排序，设置排序条件
        std::optional<std::size_t> field_index = collection->get_field_index(order_column.value());
        if (field_index.has_value()) {
            // Order 需要 std::uint8_t，进行类型转换
            if (field_index.value() > 255) {
                result.set_is_success(false);
                result.set_message("Field index too large for ordering");
                return result;
            }
            // 获取 order_type，也需要检查是否存在
            const std::optional<Direction> & order_type_opt = update_stmt.get_order_type();
            Direction order_type = order_type_opt.value_or(Direction::ASC); // 默认 ASC
            query.set_order(Order(static_cast<std::uint8_t>(field_index.value()), order_type));
        } else {
            result.set_is_success(false);
            result.set_message("Unknown column: '" + order_column.value() + "'");
            return result;
        }
    }
    // 设置 limit 条件
    const std::optional<std::size_t> & limit = update_stmt.get_limit();
    if (limit.has_value()) {
        query.set_limit(Limit(limit.value()));
    }
    
    // 执行查询，获取符合条件的实体
    std::vector<std::unique_ptr<Entity>> entities = collection->query(query);
    
    // 更新查询结果中的实体
    std::size_t updated_count = 0;
    for (const auto & entity : entities) {
        MutationResult update_result = collection->update(entity->get_id(), update_fields);
        if (update_result.is_success()) {
            updated_count++;
        }
    }
    
    result.set_is_success(true);
    result.set_affected_count(updated_count);
    result.set_message("Updated " + std::to_string(updated_count) + " row(s)");
    return result;
}

ExecutorResult Executor::execute_create(const CreateStmt & create_stmt)
{
    // 获取创建类型
    CreateStmt::CreateType create_type = create_stmt.get_create_type();

    // 根据不同创建类型执行不同创建操作
    switch (create_type) {
        case CreateStmt::CreateType::DATABASE:
            return execute_create_database(create_stmt);
        case CreateStmt::CreateType::COLLECTION:
            return execute_create_collection(create_stmt);
        case CreateStmt::CreateType::INDEX:
            return execute_create_index(create_stmt);
        case CreateStmt::CreateType::VINDEX:
            return execute_create_vindex(create_stmt);
        default: {
            ExecutorResult result;
            result.set_is_success(false);
            result.set_message("Unsupported create type: " + std::to_string(static_cast<std::uint8_t>(create_type)));
            return result;
        }
    }
}

ExecutorResult Executor::execute_drop(const DropStmt & drop_stmt)
{
    // 获取删除类型
    DropStmt::DropType drop_type = drop_stmt.get_drop_type();

    // 根据不同删除类型执行不同删除操作
    switch (drop_type) {
        case DropStmt::DropType::DATABASE:
            return execute_drop_database(drop_stmt);
        case DropStmt::DropType::COLLECTION:
            return execute_drop_collection(drop_stmt);
        case DropStmt::DropType::INDEX:
            return execute_drop_index(drop_stmt);
        case DropStmt::DropType::VINDEX:
            return execute_drop_vindex(drop_stmt);
        default: {
            ExecutorResult result;
            result.set_is_success(false);
            result.set_message("Unsupported drop type: " + std::to_string(static_cast<std::uint8_t>(drop_type)));
            return result;
        }
    }
}

ExecutorResult Executor::execute_use(const UseStmt & use_stmt)
{
    // 获取数据库名称
    const std::string & database_name = use_stmt.get_database_name();

    ExecutorResult result;

    // 检查数据库是否存在
    if (!database_manager_->has_database(database_name)) {
        result.set_is_success(false);
        result.set_message("Unknown database: '" + database_name + "'");
        return result;
    }

    // 设置当前数据库
    database_manager_->set_current_database(database_name);
    result.set_is_success(true);
    result.set_message("Database changed");

    return result;
}

ExecutorResult Executor::execute_describe(const DescribeStmt & describe_stmt)
{
    // 获取集合名
    const std::string & collection_name = describe_stmt.get_collection_name();

    ExecutorResult result;

    // 获取当前数据库
    Database * database = get_current_database();
    if (database == nullptr) {
        result.set_is_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 获取集合
    Collection * collection = database->get_collection(collection_name);

    if (collection == nullptr) {
        result.set_is_success(false);
        result.set_message("Unknown collection: '" + collection_name + "'");
        return result;
    }

    // 获取 Schema
    const std::vector<Field> & schema = collection->get_schema();

    // 格式化输出 Schema 信息
    std::string output = "Collection: " + collection_name + "\n";
    output += "Fields (" + std::to_string(schema.size()) + "):\n";
    output += "------------------------------------------------------------\n";

    // 遍历每个字段，格式化输出
    for (std::size_t i = 0; i < schema.size(); ++i) {
        const Field & field = schema[i];

        // 字段名和索引
        output += "[" + std::to_string(i) + "] " + field.get_name() + " | ";

        // 字段类型
        FieldType type = field.get_type();
        std::string type_str;
        switch (type) {
            case FieldType::TINYINT:
                type_str = "TINYINT";
                break;
            case FieldType::SMALLINT:
                type_str = "SMALLINT";
                break;
            case FieldType::INTEGER:
                type_str = "INTEGER";
                break;
            case FieldType::BIGINT:
                type_str = "BIGINT";
                break;
            case FieldType::FLOAT:
                type_str = "FLOAT";
                break;
            case FieldType::DOUBLE:
                type_str = "DOUBLE";
                break;
            case FieldType::DECIMAL:
                type_str = "DECIMAL(" + std::to_string(field.get_length()) + "," + std::to_string(field.get_precision()) + ")";
                break;
            case FieldType::CHAR:
                type_str = "CHAR(" + std::to_string(field.get_length()) + ")";
                break;
            case FieldType::VARCHAR:
                type_str = "VARCHAR(" + std::to_string(field.get_length()) + ")";
                break;
            case FieldType::BOOLEAN:
                type_str = "BOOLEAN";
                break;
            case FieldType::TIMESTAMP:
                type_str = "TIMESTAMP";
                break;
            case FieldType::ENUM: {
                type_str = "ENUM(";
                const auto & options = field.get_options();
                for (std::size_t j = 0; j < options.size(); ++j) {
                    if (j > 0) type_str += ",";
                    type_str += "'" + options[j] + "'";
                }
                type_str += ")";
                break;
            }
            case FieldType::VECTOR:
                type_str = "VECTOR(" + std::to_string(field.get_length()) + ")";
                break;
        }
        output += type_str + " | ";

        // 属性列表
        std::vector<std::string> attributes;

        // NULL 约束
        if (!field.get_is_nullable()) {
            attributes.push_back("NOT NULL");
        }

        // PRIMARY KEY
        if (field.get_is_primary()) {
            attributes.push_back("PRIMARY KEY");
        }

        // AUTO_INCREMENT
        if (field.get_is_auto_increment()) {
            attributes.push_back("AUTO_INCREMENT");
        }
        
        // 输出属性
        if (!attributes.empty()) {
            for (std::size_t j = 0; j < attributes.size(); ++j) {
                if (j > 0) output += " ";
                output += attributes[j];
            }
        }
        
        output += "\n";
    }

    result.set_is_success(true);
    result.set_message(output);

    return result;
}

ExecutorResult Executor::execute_show(const ShowStmt & show_stmt)
{
    // 获取显示类型
    ShowStmt::ShowType show_type = show_stmt.get_show_type();

    // 根据不同显示类型执行不同显示操作
    switch (show_type) {
        case ShowStmt::ShowType::DATABASES:
            return execute_show_databases();
        case ShowStmt::ShowType::COLLECTIONS:
            return execute_show_collections();
        case ShowStmt::ShowType::INDEXES:
            return execute_show_indexes(show_stmt);
        case ShowStmt::ShowType::VINDEXES:
            return execute_show_vindexes(show_stmt);
        default: {
            ExecutorResult result;
            result.set_is_success(false);
            result.set_message("Unsupported show type: " + std::to_string(static_cast<std::uint8_t>(show_type)));
            return result;
        }
    }
}

ExecutorResult Executor::execute_alter(const AlterStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_alter not implemented");
    return result;
}

ExecutorResult Executor::execute_create_database(const CreateStmt & create_stmt)
{
    // 获取数据库名称
    const std::string & database_name = create_stmt.get_object_name();
    // 获取是否跳过存在性检查
    const bool is_if_not_exists = create_stmt.get_is_if_not_exists();

    ExecutorResult result;

    // 创建数据库
    if (database_manager_->create_database(database_name)) {
        result.set_is_success(true);
        result.set_message("Database created");
    } else {
        if (is_if_not_exists) {
            result.set_is_success(true);
            result.set_message("Database already exists, skipping");
        } else {
            result.set_is_success(false);
            result.set_message("Database '" + database_name + "' already exists");
        }
    }

    return result;
}

ExecutorResult Executor::execute_create_collection(const CreateStmt & create_stmt)
{
    // 获取集合名称
    const std::string & collection_name = create_stmt.get_object_name();
    // 获取是否跳过存在性检查
    const bool is_if_not_exists = create_stmt.get_is_if_not_exists();

    ExecutorResult result;

    // 获取当前数据库
    Database * database = get_current_database();
    if (database == nullptr) {
        result.set_is_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 构造字段定义列表
    std::vector<Field> fields;
    const std::vector<ColumnDefinition> & column_definitions = create_stmt.get_column_definitions().value();
    for (const auto & column_definition : column_definitions) {
        // 处理 options：如果是 optional，有值则使用，否则传空 vector
        const auto & options_opt = column_definition.get_options();
        const std::vector<std::string> & options = options_opt.has_value() ? options_opt.value() : std::vector<std::string>{};

        FieldValue default_value = Null();
        const AstNode * default_ast = column_definition.get_default_value();
        if (default_ast != nullptr) {
            // 使用字段类型进行类型转换
            default_value = ast_to_field_value(default_ast, column_definition.get_type());
        }

        fields.emplace_back(
            column_definition.get_name(),
            column_definition.get_type(),
            column_definition.get_length(),
            column_definition.get_precision(),
            options,
            column_definition.get_is_nullable(),
            column_definition.get_is_primary(),
            column_definition.get_comment(),
            default_value,
            column_definition.get_is_auto_increment()
        );
    }

    // 创建集合
    if (database->create_collection(collection_name, fields)) {
        result.set_is_success(true);
        result.set_message("Collection created");
    } else {
        if (is_if_not_exists) {
            result.set_is_success(true);
            result.set_message("Collection already exists, skipping");
        } else {
            result.set_is_success(false);
            result.set_message("Collection '" + collection_name + "' already exists");
        }
    }

    return result;
}

ExecutorResult Executor::execute_create_index(const CreateStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_create_index not implemented");
    return result;
}

ExecutorResult Executor::execute_create_vindex(const CreateStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_create_vindex not implemented");
    return result;
}

ExecutorResult Executor::execute_drop_database(const DropStmt & drop_stmt)
{
    // 获取数据库名称
    std::string database_name = drop_stmt.get_object_name();

    // 检查是否为当前数据库
    Database * database = get_current_database();
    if (database != nullptr && database->get_name() == database_name) {
        // 是当前数据库，设置为空后再进行删除操作
        database_manager_->set_current_database("");
    }

    ExecutorResult result;

    // 删除该数据库
    if (database_manager_->drop_database(database_name)) {
        result.set_is_success(true);
        result.set_message("Database dropped");
    } else {
        result.set_is_success(false);
        result.set_message("Unknown database: '" + database_name + "'");
    }

    return result;
}

ExecutorResult Executor::execute_drop_collection(const DropStmt & drop_stmt)
{
    // 获取集合名称
    std::string collection_name = drop_stmt.get_object_name();

    ExecutorResult result;

    Database * database = get_current_database();
    if (database == nullptr) {
        result.set_is_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 删除集合
    if (database->drop_collection(collection_name)) {
        result.set_is_success(true);
        result.set_message("Collection dropped");
    } else {
        // 集合不存在
        result.set_is_success(false);
        result.set_message("Unknown collection: '" + collection_name + "'");
    }

    return result;
}

ExecutorResult Executor::execute_drop_index(const DropStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_drop_index not implemented");
    return result;
}

ExecutorResult Executor::execute_drop_vindex(const DropStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_drop_vindex not implemented");
    return result;
}

ExecutorResult Executor::execute_show_databases()
{
    ExecutorResult result;
    result.set_is_success(true);
    result.set_message("Databases:\n");
    for (const std::string & database : database_manager_->get_databases()) {
        result.set_message(database + "\n");
    }
    return result;
}

ExecutorResult Executor::execute_show_collections()
{
    ExecutorResult result;

    // 获取当前数据库
    Database * database = get_current_database();

    if (database == nullptr) {
        // 没有选择数据库
        result.set_is_success(false);
        result.set_message("No database selected");
        return result;
    }

    result.set_is_success(true);
    result.set_message("Collections:\n");
    for (const std::string & collection : database->get_collections()) {
        result.set_message(collection + "\n");
    }
    return result;
}

ExecutorResult Executor::execute_show_indexes(const ShowStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_show_indexes not implemented");
    return result;
}

ExecutorResult Executor::execute_show_vindexes(const ShowStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_show_vindexes not implemented");
    return result;
}

FieldValue Executor::ast_to_field_value(const AstNode * ast_node, std::optional<FieldType> target_field_type)
{
    if (ast_node == nullptr) {
        return Null();
    }

    // 目前只支持字面量表达式
    if (ast_node->get_type() != AstNodeType::LITERAL_EXPR) {
        // TODO: 未来可能需要支持其他表达式类型（如函数调用、计算表达式等）
        return Null();
    }

    const LiteralExpr * literal = static_cast<const LiteralExpr *>(ast_node);
    const auto & literal_value = literal->get_literal_value();
    LiteralExpr::LiteralType literal_type = literal->get_literal_type();

    // 根据字面量类型和目标字段类型进行转换
    switch (literal_type) {
        case LiteralExpr::LiteralType::INTEGER: {
            std::int64_t int_val = std::get<std::int64_t>(literal_value);

            // 如果有目标类型，进行类型转换
            if (target_field_type.has_value()) {
                switch (target_field_type.value()) {
                    case FieldType::TINYINT:
                        return static_cast<std::int8_t>(int_val);
                    case FieldType::SMALLINT:
                        return static_cast<std::int16_t>(int_val);
                    case FieldType::INTEGER:
                        return static_cast<std::int32_t>(int_val);
                    case FieldType::BIGINT:
                    case FieldType::TIMESTAMP:
                        return int_val;
                    case FieldType::FLOAT:
                        return static_cast<float>(int_val);
                    case FieldType::DOUBLE:
                        return static_cast<double>(int_val);
                    default:
                        // 对于其他类型，返回 BIGINT
                        return int_val;
                }
            }
            // 没有目标类型，默认返回 BIGINT
            return int_val;
        }

        case LiteralExpr::LiteralType::FLOAT: {
            double float_val = std::get<double>(literal_value);
            
            if (target_field_type.has_value()) {
                switch (target_field_type.value()) {
                    case FieldType::FLOAT:
                        return static_cast<float>(float_val);
                    case FieldType::DOUBLE:
                        return float_val;
                    case FieldType::TINYINT:
                        return static_cast<std::int8_t>(float_val);
                    case FieldType::SMALLINT:
                        return static_cast<std::int16_t>(float_val);
                    case FieldType::INTEGER:
                        return static_cast<std::int32_t>(float_val);
                    case FieldType::BIGINT:
                        return static_cast<std::int64_t>(float_val);
                    default:
                        return float_val;
                }
            }
            return float_val;
        }

        case LiteralExpr::LiteralType::STRING: {
            const std::string & str_val = std::get<std::string>(literal_value);
            // 字符串直接返回，适用于 CHAR, VARCHAR, ENUM
            return str_val;
        }

        case LiteralExpr::LiteralType::BOOLEAN: {
            bool bool_val = std::get<bool>(literal_value);
            return bool_val;
        }

        case LiteralExpr::LiteralType::NULL_VALUE: {
            return Null();
        }

        case LiteralExpr::LiteralType::VECTOR: {
            const std::vector<float> & vec_val = std::get<std::vector<float>>(literal_value);
            return vec_val;
        }

        default:
            return Null();
    }
}

Database * Executor::get_current_database()
{
    // 统一入口，未来可以在这里注入事务等上下文
    // 例如：检查事务状态、获取事务相关的数据库视图等
    return database_manager_->get_current_database();
}

} // namespace dreamdb
