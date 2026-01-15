#include "dreamdb/parser/ast/debug/expression_formatter.h"

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/expression/literal_expression.h"
#include "dreamdb/parser/ast/expression/column_reference_expression.h"
#include "dreamdb/parser/ast/expression/unary_expression.h"
#include "dreamdb/parser/ast/expression/binary_expression.h"
#include "dreamdb/parser/ast/expression/function_call_expression.h"
#include "dreamdb/parser/ast/expression/in_expression.h"
#include "dreamdb/parser/ast/expression/between_expression.h"
#include "dreamdb/parser/ast/expression/like_expression.h"
#include "dreamdb/parser/ast/expression/vector_expression.h"

namespace dreamdb::parser::ast
{

namespace // anonymous namespace
{

// SQL 风格字符串转义：单引号用两个单引号表示
std::string escape_sql_string(const std::string & input)
{
    std::string result;
    result.reserve(input.size());

    for (char c : input) {
        if (c == '\'') {
            result.push_back('\'');
            result.push_back('\'');
        } else {
            result.push_back(c);
        }
    }

    return result;
}

} // anonymous namespace

AstExpressionFormatter::AstExpressionFormatter() noexcept
    : oss_()
{
}

std::string AstExpressionFormatter::format(const AstExpression & expression)
{
    oss_.str("");
    oss_.clear();
    expression.accept(*this);
    return oss_.str();
}

void AstExpressionFormatter::visit(const AstLiteralExpression & expression)
{
    const AstLiteralValue & value = expression.value();
    std::visit([this](const auto & val) {
        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<T, std::int64_t>) {
            oss_ << val;
        } else if constexpr (std::is_same_v<T, double>) {
            oss_ << val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss_ << "'" << escape_sql_string(val) << "'";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss_ << (val ? "TRUE" : "FALSE");
        } else if constexpr (std::is_same_v<T, Null>) {
            oss_ << "NULL";
        }
    }, value);
}

void AstExpressionFormatter::visit(const AstColumnReferenceExpression & expression)
{
    // 格式化三段式列名
    if (expression.database_name().has_value()) {
        oss_ << expression.database_name().value() << ".";
    }
    if (expression.collection_name().has_value()) {
        oss_ << expression.collection_name().value() << ".";
    }
    oss_ << expression.column_name();
}

void AstExpressionFormatter::visit(const AstUnaryExpression & expression)
{
    switch (expression.type()) {
        case AstUnaryOperatorType::Not:
            oss_ << "NOT ";
            expression.operand().accept(*this);
            break;
        case AstUnaryOperatorType::Minus:
            oss_ << "-";
            expression.operand().accept(*this);
            break;
        case AstUnaryOperatorType::Plus:
            oss_ << "+";
            expression.operand().accept(*this);
            break;
        default:
            oss_ << "UNKNOWN UNARY OPERATOR: " << static_cast<std::uint8_t>(expression.type());
            break;
    }
}

void AstExpressionFormatter::visit(const AstBinaryExpression & expression)
{
    oss_ << "(";
    expression.left().accept(*this);
    oss_ << " ";

    switch (expression.type()) {
        case AstBinaryOperatorType::Plus:
            oss_ << "+";
            break;
        case AstBinaryOperatorType::Minus:
            oss_ << "-";
            break;
        case AstBinaryOperatorType::Multiply:
            oss_ << "*";
            break;
        case AstBinaryOperatorType::Divide:
            oss_ << "/";
            break;
        case AstBinaryOperatorType::Modulo:
            oss_ << "%";
            break;
        case AstBinaryOperatorType::Equal:
            oss_ << "=";
            break;
        case AstBinaryOperatorType::NotEqual:
            oss_ << "!=";
            break;
        case AstBinaryOperatorType::LessThan:
            oss_ << "<";
            break;
        case AstBinaryOperatorType::GreaterThan:
            oss_ << ">";
            break;
        case AstBinaryOperatorType::LessEqual:
            oss_ << "<=";
            break;
        case AstBinaryOperatorType::GreaterEqual:
            oss_ << ">=";
            break;
        case AstBinaryOperatorType::And:
            oss_ << "AND";
            break;
        case AstBinaryOperatorType::Or:
            oss_ << "OR";
            break;
        default:
            oss_ << "UNKNOWN BINARY OPERATOR: " << static_cast<std::uint8_t>(expression.type());
            break;
    }

    oss_ << " ";
    expression.right().accept(*this);
    oss_ << ")";
}

void AstExpressionFormatter::visit(const AstFunctionCallExpression & expression)
{
    oss_ << expression.function_name() << "(";

    for (std::size_t i = 0; i < expression.argument_count(); ++i) {
        expression.argument_at(i).accept(*this);
        if (i < expression.argument_count() - 1) {
            oss_ << ", ";
        }
    }

    oss_ << ")";
}

void AstExpressionFormatter::visit(const AstInExpression & expression)
{
    expression.left().accept(*this);
    if (expression.is_not()) {
        oss_ << " NOT IN (";
    } else {
        oss_ << " IN (";
    }

    // 格式化值表达式列表
    for (std::size_t i = 0; i < expression.value_count(); ++i) {
        expression.value_at(i).accept(*this);
        if (i < expression.value_count() - 1) {
            oss_ << ", ";
        }
    }

    oss_ << ")";
}

void AstExpressionFormatter::visit(const AstBetweenExpression & expression)
{
    expression.left().accept(*this);
    if (expression.is_not()) {
        oss_ << " NOT BETWEEN ";
    } else {
        oss_ << " BETWEEN ";
    }

    // 格式化起始值表达式
    expression.start().accept(*this);
    oss_ << " AND ";
    // 格式化结束值表达式
    expression.end().accept(*this);
}

void AstExpressionFormatter::visit(const AstLikeExpression & expression)
{
    expression.left().accept(*this);
    if (expression.is_not()) {
        oss_ << " NOT LIKE ";
    } else {
        oss_ << " LIKE ";
    }

    expression.pattern().accept(*this);
}

void AstExpressionFormatter::visit(const AstVectorExpression & expression)
{
    oss_ << "[";

    for (std::size_t i = 0; i < expression.element_count(); ++i) {
        expression.element_at(i).accept(*this);
        if (i < expression.element_count() - 1) {
            oss_ << ", ";
        }
    }

    oss_ << "]";
}

} // namespace dreamdb::parser::ast
