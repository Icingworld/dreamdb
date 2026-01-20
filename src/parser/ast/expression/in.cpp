#include "dreamdb/parser/ast/expression/in.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstInExpression::AstInExpression(
    std::unique_ptr<AstExpression> left,
    std::vector<std::unique_ptr<AstExpression>> values,
    bool is_not,
    std::size_t line,
    std::size_t column
)
    : AstExpression(AstExpressionType::In, line, column)
    , left_(std::move(left))
    , values_(std::move(values))
    , is_not_(is_not)
{
    // 左侧表达式不能为空
    assert(left_ != nullptr);

    // IN 表达式必须有至少一个值
    assert(!values_.empty());

    // 值列表中的每个值都不能为空
    for (const auto & value : values_) {
        assert(value != nullptr);
    }
}

std::unique_ptr<AstInExpression> AstInExpression::create(
    std::unique_ptr<AstExpression> left,
    std::vector<std::unique_ptr<AstExpression>> values,
    bool is_not,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstInExpression>(std::move(left), std::move(values), is_not, line, column);
}

const AstExpression & AstInExpression::left() const noexcept
{
    return *left_;
}

const AstExpression & AstInExpression::value_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < values_.size());

    return *values_[index];
}

bool AstInExpression::is_not() const noexcept
{
    return is_not_;
}

std::size_t AstInExpression::value_count() const noexcept
{
    return values_.size();
}

void AstInExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
