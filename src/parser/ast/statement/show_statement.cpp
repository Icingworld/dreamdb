#include "dreamdb/parser/ast/statement/show_statement.h"

#include <cassert>

#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstShowStatement::AstShowStatement(AstShowStatementOperation operation, std::size_t line, std::size_t column)
    : AstStatement(line, column)
    , operation_(std::move(operation))
{
    std::visit([](auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstShowDatabases>) {
            // 无字段，不需要校验
        } else if constexpr (std::is_same_v<T, AstShowCollections>) {
            if (op.database_name.has_value()) {
                // 数据库名称不能为空
                assert(!op.database_name->empty());
            }
        } else if constexpr (std::is_same_v<T, AstShowIndexes>) {
            // 集合名称不能为空
            assert(!op.collection_name.empty());

            if (op.database_name.has_value()) {
                // 数据库名称不能为空
                assert(!op.database_name->empty());
            }
        } else if constexpr (std::is_same_v<T, AstShowVIndexes>) {
            // 集合名称不能为空
            assert(!op.collection_name.empty());

            if (op.database_name.has_value()) {
                // 数据库名称不能为空
                assert(!op.database_name->empty());
            }
        } else {
            // 不会到达这里
            assert(false);
        }
    }, operation_);
}

std::unique_ptr<AstShowStatement> AstShowStatement::create(
    AstShowStatementOperation operation,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstShowStatement>(std::move(operation), line, column);
}

const AstShowStatementOperation & AstShowStatement::operation() const noexcept
{
    return operation_;
}

void AstShowStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
