#include "dreamdb/parser/ast/statement/alter.h"

#include <cassert>

#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstAlterStatement::AstAlterStatement(
    std::string collection_name,
    AstAlterStatementOperation operation,
    std::size_t line,
    std::size_t column
)
    : AstStatement(line, column)
    , collection_name_(std::move(collection_name))
    , operation_(std::move(operation))
{
    // 集合名称不能为空
    assert(!collection_name_.empty());

    std::visit([](auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstAlterAddColumn>) {
            // 列定义已经通过构造函数验证
        } else if constexpr (std::is_same_v<T, AstAlterDropColumn>) {
            // 列名不能为空
            assert(!op.column_name.empty());
        } else if constexpr (std::is_same_v<T, AstAlterModifyColumn>) {
            // 列定义已经通过构造函数验证
        } else if constexpr (std::is_same_v<T, AstAlterRenameColumn>) {
            // 旧列名不能为空
            assert(!op.old_name.empty());
            // 新列名不能为空
            assert(!op.new_name.empty());
        } else {
            // 不会到达这里
            assert(false);
        }
    }, operation_);
}

std::unique_ptr<AstAlterStatement> AstAlterStatement::create(
    std::string collection_name,
    AstAlterStatementOperation operation,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstAlterStatement>(std::move(collection_name), std::move(operation), line, column);
}

const std::string & AstAlterStatement::collection_name() const noexcept
{
    return collection_name_;
}

const AstAlterStatementOperation & AstAlterStatement::operation() const noexcept
{
    return operation_;
}

void AstAlterStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
