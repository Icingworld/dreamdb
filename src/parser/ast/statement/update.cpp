#include "dreamdb/parser/ast/statement/update.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/statement/statement_visitor.h"

namespace dreamdb::parser::ast
{

AstUpdateAssignment::AstUpdateAssignment(std::string column_name, std::unique_ptr<AstExpression> value)
    : column_name(std::move(column_name))
    , value(std::move(value))
{
    // 列名不能为空
    assert(!this->column_name.empty());

    // 值表达式不能为空
    assert(this->value != nullptr);
}

AstUpdateStatement::AstUpdateStatement(
    std::string collection_name,
    std::vector<AstUpdateAssignment> assignments,
    std::unique_ptr<AstExpression> where,
    std::size_t line,
    std::size_t column
)
    : AstStatement(AstStatementType::Update, line, column)
    , collection_name_(std::move(collection_name))
    , assignments_(std::move(assignments))
    , where_(std::move(where))
{
    // 集合名称不能为空
    assert(!collection_name_.empty());

    // 赋值列表不能为空
    assert(!assignments_.empty());

    // 每个赋值项的列名和值表达式都不能为空
    for (const auto & assignment : assignments_) {
        assert(!assignment.column_name.empty());
        assert(assignment.value != nullptr);
    }
}

AstUpdateStatement::~AstUpdateStatement() noexcept = default;

std::unique_ptr<AstUpdateStatement> AstUpdateStatement::create(
    std::string collection_name,
    std::vector<AstUpdateAssignment> assignments,
    std::unique_ptr<AstExpression> where,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstUpdateStatement>(
        std::move(collection_name),
        std::move(assignments),
        std::move(where),
        line,
        column
    );
}

const std::string & AstUpdateStatement::collection_name() const noexcept
{
    return collection_name_;
}

const AstUpdateAssignment & AstUpdateStatement::assignment_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < assignments_.size());

    return assignments_[index];
}

std::size_t AstUpdateStatement::assignment_count() const noexcept
{
    return assignments_.size();
}

bool AstUpdateStatement::has_where() const noexcept
{
    return where_ != nullptr;
}

const AstExpression * AstUpdateStatement::where() const noexcept
{
    return where_.get();
}

const AstExpression & AstUpdateStatement::where_ref() const noexcept
{
    // WHERE 子句必须存在
    assert(where_ != nullptr);

    return *where_;
}

void AstUpdateStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
