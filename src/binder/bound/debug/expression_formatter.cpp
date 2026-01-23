#include "dreamdb/binder/bound/debug/expression_formatter.h"

#include <iomanip>
#include <sstream>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/binder/bound/expression/column_reference.h"
#include "dreamdb/binder/bound/expression/constant.h"
#include "dreamdb/binder/bound/expression/binary.h"
#include "dreamdb/binder/bound/expression/unary.h"
#include "dreamdb/binder/bound/expression/function_call.h"
#include "dreamdb/binder/bound/expression/in.h"
#include "dreamdb/binder/bound/expression/between.h"
#include "dreamdb/binder/bound/expression/like.h"
#include "dreamdb/common/null.h"

namespace dreamdb::binder::bound
{

namespace
{

/**
 * @brief 格式化 FieldValue
 */
std::string format_field_value(const dreamdb::FieldValue & value)
{
    return std::visit([](const auto & v) -> std::string {
        using T = std::decay_t<decltype(v)>;

        if constexpr (std::is_same_v<T, std::int8_t>) {
            return std::to_string(static_cast<int>(v));
        } else if constexpr (std::is_same_v<T, std::int16_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, std::int32_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            return std::to_string(v);
        } else if constexpr (std::is_same_v<T, float>) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << v;
            std::string s = oss.str();
            // 移除尾部的零和小数点
            auto pos = s.find_last_not_of('0');
            if (pos != std::string::npos && pos < s.length() - 1) {
                s.erase(pos + 1);
            }
            // 如果最后是小数点，也移除
            if (!s.empty() && s.back() == '.') {
                s.pop_back();
            }
            return s;
        } else if constexpr (std::is_same_v<T, double>) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(15) << v;
            std::string s = oss.str();
            // 移除尾部的零和小数点
            auto pos = s.find_last_not_of('0');
            if (pos != std::string::npos && pos < s.length() - 1) {
                s.erase(pos + 1);
            }
            // 如果最后是小数点，也移除
            if (!s.empty() && s.back() == '.') {
                s.pop_back();
            }
            return s;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "'" + v + "'";
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            std::ostringstream oss;
            oss << "[";
            for (std::size_t i = 0; i < v.size(); ++i) {
                if (i > 0) {
                    oss << ", ";
                }
                oss << std::fixed << std::setprecision(6) << v[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, dreamdb::Null>) {
            return "NULL";
        } else {
            return "?";
        }
    }, value);
}

/**
 * @brief 格式化二元运算符类型
 */
std::string format_binary_operator(BoundBinaryOperatorType op_type)
{
    switch (op_type) {
        case BoundBinaryOperatorType::Plus:
            return "+";
        case BoundBinaryOperatorType::Minus:
            return "-";
        case BoundBinaryOperatorType::Multiply:
            return "*";
        case BoundBinaryOperatorType::Divide:
            return "/";
        case BoundBinaryOperatorType::Modulo:
            return "%";
        case BoundBinaryOperatorType::Equal:
            return "=";
        case BoundBinaryOperatorType::NotEqual:
            return "!=";
        case BoundBinaryOperatorType::LessThan:
            return "<";
        case BoundBinaryOperatorType::GreaterThan:
            return ">";
        case BoundBinaryOperatorType::LessEqual:
            return "<=";
        case BoundBinaryOperatorType::GreaterEqual:
            return ">=";
        case BoundBinaryOperatorType::And:
            return "AND";
        case BoundBinaryOperatorType::Or:
            return "OR";
        default:
            return "?";
    }
}

/**
 * @brief 格式化一元运算符类型
 */
std::string format_unary_operator(BoundUnaryOperatorType op_type)
{
    switch (op_type) {
        case BoundUnaryOperatorType::Not:
            return "NOT";
        case BoundUnaryOperatorType::Minus:
            return "-";
        case BoundUnaryOperatorType::Plus:
            return "+";
        default:
            return "?";
    }
}

/**
 * @brief 判断二元运算符是否需要括号
 */
bool needs_parentheses_for_binary(BoundBinaryOperatorType op_type)
{
    // 逻辑运算符优先级最低，需要括号
    return op_type == BoundBinaryOperatorType::And || op_type == BoundBinaryOperatorType::Or;
}

} // anonymous namespace

BoundExpressionFormatter::BoundExpressionFormatter() noexcept
    : oss_()
{
}

std::string BoundExpressionFormatter::format(const BoundExpression & expression)
{
    oss_.str("");
    oss_.clear();
    expression.accept(*this);
    return oss_.str();
}

void BoundExpressionFormatter::visit(const BoundColumnReferenceExpression & column_reference_expression)
{
    oss_ << "column_id:" << column_reference_expression.column_id();
}

void BoundExpressionFormatter::visit(const BoundConstantExpression & constant_expression)
{
    oss_ << format_field_value(constant_expression.value());
}

void BoundExpressionFormatter::visit(const BoundFunctionCallExpression & function_call_expression)
{
    oss_ << function_call_expression.function_name() << "(";

    // 格式化参数
    for (std::size_t i = 0; i < function_call_expression.argument_count(); ++i) {
        if (i > 0) {
            oss_ << ", ";
        }
        function_call_expression.argument_at(i).accept(*this);
    }

    oss_ << ")";
}

void BoundExpressionFormatter::visit(const BoundInExpression & in_expression)
{
    // 格式化左侧表达式
    in_expression.left().accept(*this);

    if (in_expression.is_not()) {
        oss_ << " NOT";
    }

    oss_ << " IN (";

    // 格式化值列表
    for (std::size_t i = 0; i < in_expression.value_count(); ++i) {
        if (i > 0) {
            oss_ << ", ";
        }
        in_expression.value_at(i).accept(*this);
    }

    oss_ << ")";
}

void BoundExpressionFormatter::visit(const BoundBetweenExpression & between_expression)
{
    // 格式化左侧表达式
    between_expression.left().accept(*this);

    if (between_expression.is_not()) {
        oss_ << " NOT";
    }

    oss_ << " BETWEEN ";

    // 格式化起始值
    between_expression.start().accept(*this);

    oss_ << " AND ";

    // 格式化结束值
    between_expression.end().accept(*this);
}

void BoundExpressionFormatter::visit(const BoundLikeExpression & like_expression)
{
    // 格式化左侧表达式
    like_expression.left().accept(*this);

    if (like_expression.is_not()) {
        oss_ << " NOT";
    }

    oss_ << " LIKE ";

    // 格式化模式表达式
    like_expression.pattern().accept(*this);
}

void BoundExpressionFormatter::visit(const BoundUnaryExpression & unary_expression)
{
    oss_ << format_unary_operator(unary_expression.operator_type());

    // 对于 NOT 运算符，后面加空格；对于 +/- 运算符，直接连接
    if (unary_expression.operator_type() == BoundUnaryOperatorType::Not) {
        oss_ << " ";
    }

    // 格式化操作数
    unary_expression.operand().accept(*this);
}

void BoundExpressionFormatter::visit(const BoundBinaryExpression & binary_expression)
{
    bool needs_left_paren = needs_parentheses_for_binary(binary_expression.operator_type());

    if (needs_left_paren) {
        oss_ << "(";
    }

    // 格式化左操作数
    binary_expression.left().accept(*this);

    oss_ << " " << format_binary_operator(binary_expression.operator_type()) << " ";

    // 格式化右操作数
    binary_expression.right().accept(*this);

    if (needs_left_paren) {
        oss_ << ")";
    }
}

} // namespace dreamdb::binder::bound
