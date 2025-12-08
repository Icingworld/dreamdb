#include "dreamdb/executor/executor.h"

#include "dreamdb/parser/ast/select_stmt.h"
#include "dreamdb/parser/ast/delete_stmt.h"
#include "dreamdb/parser/ast/insert_stmt.h"
#include "dreamdb/parser/ast/update_stmt.h"
#include "dreamdb/parser/ast/create_stmt.h"
#include "dreamdb/parser/ast/drop_stmt.h"
#include "dreamdb/parser/ast/use_stmt.h"
#include "dreamdb/parser/ast/literal_expr.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

ExecutorResult::ExecutorResult()
    : success(false)
    , message()
    , affected_count(0)
    , rows()
{
}

void ExecutorResult::set_success(bool success) noexcept
{
    this->success = success;
}

bool ExecutorResult::is_success() const noexcept
{
    return success;
}

void ExecutorResult::set_message(const std::string & message)
{
    this->message = message;
}

const std::string & ExecutorResult::get_message() const noexcept
{
    return message;
}

void ExecutorResult::set_affected_count(std::size_t count) noexcept
{
    affected_count = count;
}

std::size_t ExecutorResult::get_affected_count() const noexcept
{
    return affected_count;
}

void ExecutorResult::add_row(Entity && entity)
{
    rows.emplace_back(std::move(entity));
}

std::size_t ExecutorResult::get_row_count() const noexcept
{
    return rows.size();
}

const std::vector<Entity> & ExecutorResult::get_rows() const noexcept
{
    return rows;
}

void ExecutorResult::clear() noexcept
{
    success = false;
    message.clear();
    affected_count = 0;
    rows.clear();
}

ExecutorResult Executor::execute(const AstNode & ast)
{
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
        default: {
            ExecutorResult result;
            result.set_success(false);
            result.set_message("Unsupported AST node type: " + ast.debug_string());
            return result;
        }
    }
}

ExecutorResult Executor::execute_select(const SelectStmt & select_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_message("Executor::execute_select not implemented");
    return result;
}

ExecutorResult Executor::execute_delete(const DeleteStmt & delete_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_message("Executor::execute_delete not implemented");
    return result;
}

ExecutorResult Executor::execute_insert(const InsertStmt &)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_message("Executor::execute_insert not implemented");
    return result;
}

ExecutorResult Executor::execute_update(const UpdateStmt & update_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_message("Executor::execute_update not implemented");
    return result;
}

ExecutorResult Executor::execute_create(const CreateStmt & create_stmt)
{
    // 获取创建类型
    CreateStmt::ObjectType object_type = create_stmt.get_object_type();
    switch (object_type) {
        case CreateStmt::ObjectType::DATABASE:
            return execute_create_database(create_stmt);
        case CreateStmt::ObjectType::COLLECTION:
            return execute_create_collection(create_stmt);
        case CreateStmt::ObjectType::INDEX:
            return execute_create_index(create_stmt);
        default:
            ExecutorResult result;
            result.set_success(false);
            result.set_message("Unsupported object type: " + std::to_string(static_cast<std::uint8_t>(object_type)));
            return result;
    }
}

ExecutorResult Executor::execute_drop(const DropStmt & drop_stmt)
{
    // 获取删除类型
    DropStmt::ObjectType object_type = drop_stmt.get_object_type();
    switch (object_type) {
        case DropStmt::ObjectType::COLLECTION:
            return execute_drop_collection(drop_stmt);
        case DropStmt::ObjectType::INDEX:
            return execute_drop_index(drop_stmt);
        default:
            ExecutorResult result;
            result.set_success(false);
            result.set_message("Unsupported object type: " + std::to_string(static_cast<std::uint8_t>(object_type)));
            return result;
    }
}

ExecutorResult Executor::execute_use(const UseStmt & use_stmt)
{
    // 获取数据库名称
    std::string database_name = use_stmt.get_database_name();

    ExecutorResult result;

    // 检查数据库是否存在
    if (!database_manager_.has_database(database_name)) {
        result.set_success(false);
        result.set_message("Database: " + database_name + " does not exists");
        return result;
    }

    // 设置当前数据库
    database_manager_.set_current_database(database_name);
    result.set_success(true);
    result.set_message("Database switched to '" + database_name + "'");

    return result;
}

ExecutorResult Executor::execute_create_database(const CreateStmt & create_stmt)
{
    // 获取数据库名称
    std::string database_name = create_stmt.get_object_name();

    ExecutorResult result;

    // 创建数据库
    if (database_manager_.create_database(database_name)) {
        result.set_success(true);
        result.set_message("Database: " + database_name + " created successfully");
    } else {
        result.set_success(false);
        result.set_message("Database: " + database_name + " already exists");
    }

    return result;
}

ExecutorResult Executor::execute_create_collection(const CreateStmt & create_stmt)
{
    // 获取集合名称
    std::string collection_name = create_stmt.get_object_name();

    ExecutorResult result;

    // 获取当前数据库
    Database * database = database_manager_.get_current_database();
    if (database == nullptr) {
        result.set_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 获取集合管理器
    CollectionManager & collection_manager = database->get_collection_manager();
    // 检查集合是否存在
    if (collection_manager.has_collection(collection_name)) {
        result.set_success(false);
        result.set_message("Collection: " + collection_name + " already exists");
        return result;
    }

    // 获取列定义列表
    const std::vector<ColumnDefinition> & column_definitions = create_stmt.get_column_definitions();
    if (column_definitions.empty()) {
        result.set_success(false);
        result.set_message("Collection must have at least one column");
        return result;
    }

    std::vector<Field> fields;
    fields.reserve(column_definitions.size());

    // 转换为字段列表
    for (const ColumnDefinition & column_definition : column_definitions) {
        // 处理默认值
        FieldValue default_value = NullType{};
        
        if (column_definition.has_default_value()) {
            const AstNode * default_expr = column_definition.get_default_value();
            
            // 只支持 LiteralExpr 作为默认值
            if (default_expr->get_type() == AstNodeType::LITERAL_EXPR) {
                const LiteralExpr * literal = static_cast<const LiteralExpr *>(default_expr);
                const LiteralValue & literal_value = literal->get_value();
                
                // 将 LiteralValue 转换为 FieldValue
                if (literal->is_null()) {
                    default_value = NullType{};
                } else {
                    std::visit([&default_value](const auto & val) {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, int64_t>) {
                            default_value = val;
                        } else if constexpr (std::is_same_v<T, double>) {
                            default_value = val;
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            default_value = val;
                        } else if constexpr (std::is_same_v<T, bool>) {
                            default_value = val;
                        } else if constexpr (std::is_same_v<T, NullType>) {
                            default_value = NullType{};
                        }
                    }, literal_value);
                }
            } else {
                result.set_success(false);
                result.set_message("Default value must be a literal expression");
                return result;
            }
        }

        // 创建 Field
        fields.emplace_back(
            column_definition.get_name(),
            column_definition.get_type(),
            column_definition.get_length(),
            column_definition.get_precision(),
            column_definition.is_nullable(),
            column_definition.is_primary_key(),
            "",  // comment
            default_value,
            column_definition.is_auto_increment()
        );
    }

    // 创建集合
    Collection * collection = collection_manager.create_collection(collection_name, fields);

    if (collection == nullptr) {
        result.set_success(false);
        result.set_message("Collection: " + collection_name + " already exists");
        return result;
    }

    result.set_success(true);
    result.set_message("Collection: " + collection_name + " created successfully");
    return result;
}

ExecutorResult Executor::execute_create_index(const CreateStmt & create_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_message("Executor::execute_create_index not implemented");
    return result;
}

ExecutorResult Executor::execute_drop_database(const DropStmt & drop_stmt)
{
    // 获取数据库名称
    std::string database_name = drop_stmt.get_object_name();

    ExecutorResult result;

    // 删除该数据库
    if (database_manager_.drop_database(database_name)) {
        result.set_success(true);
        result.set_message("Database '" + database_name + "' dropped successfully");
    } else {
        result.set_success(false);
        result.set_message("Database: " + database_name + " does not exists");
    }

    return result;
}

ExecutorResult Executor::execute_drop_collection(const DropStmt & drop_stmt)
{
    // 获取集合名称
    std::string collection_name = drop_stmt.get_object_name();

    ExecutorResult result;

    Database * database = database_manager_.get_current_database();
    if (database == nullptr) {
        result.set_success(false);
        result.set_message("No database selected");
        return result;
    }

    // 获取集合管理器
    CollectionManager & collection_manager = database->get_collection_manager();

    // 删除集合
    if (collection_manager.drop_collection(collection_name)) {
        result.set_success(true);
        result.set_message("Collection: " + collection_name + " dropped successfully");
    } else {
        // 集合不存在
        result.set_success(false);
        result.set_message("Collection: " + collection_name + " does not exists");
    }

    return result;
}

ExecutorResult Executor::execute_drop_index(const DropStmt & drop_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_message("Executor::execute_drop_index not implemented");
    return result;
}

} // namespace dreamdb
