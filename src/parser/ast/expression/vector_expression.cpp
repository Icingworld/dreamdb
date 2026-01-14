#include "dreamdb/parser/ast/expression/vector_expression.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstVectorExpression::AstVectorExpression(
    std::vector<std::unique_ptr<AstExpression>> elements,
    std::size_t line,
    std::size_t column
)
    : AstExpression(AstExpressionType::Vector, line, column)
    , elements_(std::move(elements))
{
    // 向量表达式必须有至少一个元素
    assert(!elements_.empty());

    // 元素列表中的每个元素都不能为空
    for (const auto & element : elements_) {
        assert(element != nullptr);
    }
}

std::unique_ptr<AstVectorExpression> AstVectorExpression::create(
    std::vector<std::unique_ptr<AstExpression>> elements,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstVectorExpression>(std::move(elements), line, column);
}

const AstExpression & AstVectorExpression::element_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < elements_.size());

    return *elements_[index];
}

std::size_t AstVectorExpression::element_count() const noexcept
{
    return elements_.size();
}

void AstVectorExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
