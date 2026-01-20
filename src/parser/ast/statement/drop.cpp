#include "dreamdb/parser/ast/statement/drop.h"

#include <cassert>

#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstDropStatement::AstDropStatement(
    AstDropStatementOperation operation,
    bool if_exists,
    std::size_t line,
    std::size_t column
)
    : AstStatement(line, column)
    , operation_(std::move(operation))
    , if_exists_(if_exists)
{
    std::visit([](auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstDropDatabase>) {
            // 数据库名称不能为空
            assert(!op.database_name.empty());
        } else if constexpr (std::is_same_v<T, AstDropCollection>) {
            // 集合名称不能为空
            assert(!op.collection_name.empty());
        } else if constexpr (std::is_same_v<T, AstDropIndex>) {
            // 索引名称不能为空
            assert(!op.index_name.empty());

            // 集合名称不能为空
            assert(!op.collection_name.empty());
        } else if constexpr (std::is_same_v<T, AstDropVIndex>) {
            // 向量索引名称不能为空
            assert(!op.vindex_name.empty());

            // 集合名称不能为空
            assert(!op.collection_name.empty());
        } else {
            // 不会到达这里
            assert(false);
        }
    }, operation_);
}

std::unique_ptr<AstDropStatement> AstDropStatement::create(
    AstDropStatementOperation operation,
    bool if_exists,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstDropStatement>(
        std::move(operation), if_exists, line, column
    );
}

const AstDropStatementOperation & AstDropStatement::operation() const noexcept
{
    return operation_;
}

bool AstDropStatement::if_exists() const noexcept
{
    return if_exists_;
}

void AstDropStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
