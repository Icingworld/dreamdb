#include "dreamdb/executor/executor.h"

#include <algorithm>
#include <sstream>
#include <iomanip>

#include "dreamdb/schema/database_manager.h"
#include "dreamdb/schema/database.h"
#include "dreamdb/schema/collection.h"
#include "dreamdb/schema/index_meta.h"
#include "dreamdb/schema/field.h"
#include "dreamdb/catalog/catalog.h"
#include "dreamdb/catalog/catalog_database_entry.h"
#include "dreamdb/catalog/catalog_collection_entry.h"
#include "dreamdb/catalog/catalog_column_entry.h"
#include "dreamdb/catalog/catalog_index_entry.h"
#include "dreamdb/catalog/catalog_vindex_entry.h"
#include "dreamdb/catalog/logical_type.h"
#include "dreamdb/common/type.h"

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

MutationResult Executor::execute_insert(const BoundInsertStatement & /*insert_statement*/)
{
    // TODO: 实现 INSERT 执行逻辑
    return MutationResult::make_failure("INSERT execution not implemented yet");
}

} // namespace dreamdb
