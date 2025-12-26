#include "dreamdb/executor/executor.h"

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

ExecutorResult Executor::execute_delete(const DeleteStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_delete not implemented");
    return result;
}

ExecutorResult Executor::execute_insert(const InsertStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_insert not implemented");
    return result;
}

ExecutorResult Executor::execute_update(const UpdateStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_update not implemented");
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

    // 获取集合
    Collection * collection = get_collection(collection_name);

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
    std::string database_name = create_stmt.get_object_name();

    ExecutorResult result;

    // 创建数据库
    if (database_manager_->create_database(database_name)) {
        result.set_is_success(true);
        result.set_message("Database created");
    } else {
        result.set_is_success(false);
        result.set_message("Database '" + database_name + "' already exists");
    }

    return result;
}

ExecutorResult Executor::execute_create_collection(const CreateStmt &)
{
    ExecutorResult result;
    result.set_is_success(false);
    result.set_message("Executor::execute_create_collection not implemented");
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

Database * Executor::get_current_database()
{
    // 统一入口，未来可以在这里注入事务等上下文
    // 例如：检查事务状态、获取事务相关的数据库视图等
    return database_manager_->get_current_database();
}

Collection * Executor::get_collection(const std::string & name)
{
    // 统一入口，未来可以在这里注入事务等上下文
    // 例如：返回事务感知的 Collection 包装器等

    Database * database = get_current_database();
    if (database == nullptr) {
        return nullptr;
    }

    return database->get_collection(name);
}

} // namespace dreamdb
