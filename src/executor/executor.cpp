#include "dreamdb/executor/executor.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

#include "dreamdb/schema/database_manager.h"
#include "dreamdb/catalog/catalog.h"

namespace dreamdb
{

namespace
{

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

} // anonymous namespace

Executor::Executor(std::unique_ptr<DatabaseManager> database_manager)
    : database_manager_(std::move(database_manager))
{
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

MutationResult Executor::execute_select(const PhysicalSelectPlanNode & /*select_plan*/)
{
    // TODO: 实现 SELECT 执行逻辑
    return MutationResult::make_failure("SELECT execution not implemented yet");
}

MutationResult Executor::execute_update(const PhysicalUpdatePlanNode & /*update_plan*/)
{
    // TODO: 实现 UPDATE 执行逻辑
    return MutationResult::make_failure("UPDATE execution not implemented yet");
}

MutationResult Executor::execute_delete(const PhysicalDeletePlanNode & /*delete_plan*/)
{
    // TODO: 实现 DELETE 执行逻辑
    return MutationResult::make_failure("DELETE execution not implemented yet");
}

MutationResult Executor::execute_use(const BoundUseStatement & use_statement)
{
    database_manager_->set_current_database(use_statement.database_id);
    MutationResult result = MutationResult::make_success();
    result.set_message("Database changed.");
    return result;
}

MutationResult Executor::execute_create(const BoundCreateStatement & /*create_statement*/)
{
    // TODO: 实现 CREATE 执行逻辑
    return MutationResult::make_failure("CREATE execution not implemented yet");
}

MutationResult Executor::execute_drop(const BoundDropStatement & /*drop_statement*/)
{
    // TODO: 实现 DROP 执行逻辑
    return MutationResult::make_failure("DROP execution not implemented yet");
}

MutationResult Executor::execute_alter(const BoundAlterStatement & /*alter_statement*/)
{
    // TODO: 实现 ALTER 执行逻辑
    return MutationResult::make_failure("ALTER execution not implemented yet");
}

MutationResult Executor::execute_describe(const BoundDescribeStatement & /*describe_statement*/)
{
    // TODO: 实现 DESCRIBE 执行逻辑
    return MutationResult::make_failure("DESCRIBE execution not implemented yet");
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

MutationResult Executor::execute_insert(const BoundInsertStatement & /*insert_statement*/)
{
    // TODO: 实现 INSERT 执行逻辑
    return MutationResult::make_failure("INSERT execution not implemented yet");
}

} // namespace dreamdb
