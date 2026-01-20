#include "dreamdb/parser/ast/expression/literal.h"

#include "dreamdb/parser/ast/expression/expression_visitor.h"

namespace dreamdb::parser::ast
{

AstLiteralExpression::AstLiteralExpression(AstLiteralValue value, std::size_t line, std::size_t column)
    : AstExpression(line, column)
    , value_(std::move(value))
{
}

std::unique_ptr<AstLiteralExpression> AstLiteralExpression::create_integer(
    std::int64_t value,
    std::size_t line,
    std::size_t column
)
{
    return std::unique_ptr<AstLiteralExpression>(
        new AstLiteralExpression(AstLiteralValue(value), line, column)
    );
}

std::unique_ptr<AstLiteralExpression> AstLiteralExpression::create_float(
    double value,
    std::size_t line,
    std::size_t column
)
{
    return std::unique_ptr<AstLiteralExpression>(
        new AstLiteralExpression(AstLiteralValue(value), line, column)
    );
}

std::unique_ptr<AstLiteralExpression> AstLiteralExpression::create_string(
    std::string value,
    std::size_t line,
    std::size_t column
)
{
    return std::unique_ptr<AstLiteralExpression>(
        new AstLiteralExpression(AstLiteralValue(value), line, column)
    );
}

std::unique_ptr<AstLiteralExpression> AstLiteralExpression::create_boolean(
    bool value,
    std::size_t line,
    std::size_t column
)
{
    return std::unique_ptr<AstLiteralExpression>(
        new AstLiteralExpression(AstLiteralValue(value), line, column)
    );
}

std::unique_ptr<AstLiteralExpression> AstLiteralExpression::create_null(
    std::size_t line,
    std::size_t column
)
{
    return std::unique_ptr<AstLiteralExpression>(
        new AstLiteralExpression(AstLiteralValue(Null()), line, column)
    );
}

const AstLiteralValue & AstLiteralExpression::value() const noexcept
{
    return value_;
}

void AstLiteralExpression::accept(AstExpressionVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
