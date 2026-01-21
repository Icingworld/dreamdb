#include "dreamdb/parser/ast/statement/create.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/statement/visitor.h"

namespace dreamdb::parser::ast
{

AstVIndexWithOption::AstVIndexWithOption(std::string key, std::unique_ptr<AstExpression> value)
    : key(std::move(key))
    , value(std::move(value))
{
    // 键不能为空
    assert(!this->key.empty());

    // 值表达式不能为空
    assert(this->value != nullptr);
}

AstCreateStatement::~AstCreateStatement() noexcept = default;

AstCreateStatement::AstCreateStatement(
    AstCreateStatementOperation operation,
    bool if_not_exists,
    std::size_t line,
    std::size_t column
)
    : AstStatement(AstStatementType::Create, line, column)
    , operation_(std::move(operation))
    , if_not_exists_(if_not_exists)
{
    std::visit([](auto & op) {
        using T = std::decay_t<decltype(op)>;

        if constexpr (std::is_same_v<T, AstCreateDatabase>) {
            // 数据库名称不能为空
            assert(!op.database_name.empty());
        } else if constexpr (std::is_same_v<T, AstCreateCollection>) {
            // 集合名称不能为空
            assert(!op.collection_name.empty());

            // 列定义列表不能为空
            assert(!op.column_definitions.empty());
        } else if constexpr (std::is_same_v<T, AstCreateIndex>) {
            // 索引名称不能为空
            assert(!op.index_name.empty());

            // 集合名称不能为空
            assert(!op.collection_name.empty());

            // 列名列表不能为空
            assert(!op.column_names.empty());
        } else if constexpr (std::is_same_v<T, AstCreateVIndex>) {
            // 向量索引名称不能为空
            assert(!op.vindex_name.empty());

            // 集合名称不能为空
            assert(!op.collection_name.empty());

            // 列名不能为空
            assert(!op.column_name.empty());
        } else {
            // 不会到达这里
            assert(false);
        }
    }, operation_);
}

std::unique_ptr<AstCreateStatement> AstCreateStatement::create(
    AstCreateStatementOperation operation,
    bool if_not_exists,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstCreateStatement>(
        std::move(operation), if_not_exists, line, column
    );
}

const AstCreateStatementOperation & AstCreateStatement::operation() const noexcept
{
    return operation_;
}

bool AstCreateStatement::if_not_exists() const noexcept
{
    return if_not_exists_;
}

void AstCreateStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast

