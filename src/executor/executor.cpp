#include "dreamdb/executor/executor.h"

#include <stdexcept>

#include "dreamdb/parser/ast/select_stmt.h"
#include "dreamdb/parser/ast/delete_stmt.h"
#include "dreamdb/parser/ast/insert_stmt.h"
#include "dreamdb/parser/ast/update_stmt.h"
#include "dreamdb/parser/ast/create_stmt.h"
#include "dreamdb/parser/ast/drop_stmt.h"

namespace dreamdb
{

ExecutorResult::ExecutorResult()
    : success(false)
    , error_message()
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

void ExecutorResult::set_error_message(const std::string & message)
{
    error_message = message;
}

const std::string & ExecutorResult::get_error_message() const noexcept
{
    return error_message;
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
    error_message.clear();
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
        default: {
            ExecutorResult result;
            result.set_success(false);
            result.set_error_message("Unsupported AST node type: " + ast.debug_string());
            return result;
        }
    }
}

ExecutorResult Executor::execute_select(const SelectStmt & select_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_error_message("Executor::execute_select not implemented");
    return result;
}

ExecutorResult Executor::execute_delete(const DeleteStmt & delete_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_error_message("Executor::execute_delete not implemented");
    return result;
}

ExecutorResult Executor::execute_insert(const InsertStmt &)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_error_message("Executor::execute_insert not implemented");
    return result;
}

ExecutorResult Executor::execute_update(const UpdateStmt & update_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_error_message("Executor::execute_update not implemented");
    return result;
}

ExecutorResult Executor::execute_create(const CreateStmt & create_stmt)
{
    ExecutorResult result;
    result.set_success(false);
    result.set_error_message("Executor::execute_create not implemented");
    return result;
}

ExecutorResult Executor::execute_drop(const DropStmt & drop_stmt)
{
    // 获取 Collection 名称
    std::string collection_name = drop_stmt.get_object_name();

    // TODO

    return ExecutorResult();
}

} // namespace dreamdb
