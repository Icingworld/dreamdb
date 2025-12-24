#include "dreamdb/parser/ast/unary_expr.h"

#include <sstream>

namespace dreamdb
{

UnaryExpr::UnaryExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::UNARY_EXPR, line, column)
    , operator_type_(OperatorType::DB_NOT)
    , operand_(nullptr)
{
}

void UnaryExpr::set_operator_type(OperatorType operator_type) noexcept
{
    operator_type_ = operator_type;
}

void UnaryExpr::set_operand(std::unique_ptr<AstNode> operand) noexcept
{
    operand_ = std::move(operand);
}

UnaryExpr::OperatorType UnaryExpr::get_operator_type() const noexcept
{
    return operator_type_;
}

const AstNode * UnaryExpr::get_operand() const noexcept
{
    return operand_.get();
}

std::string UnaryExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "UnaryExpr(op=";

    switch (operator_type_) {
        case OperatorType::DB_MINUS:
            oss << "MINUS";
            break;
        case OperatorType::DB_PLUS:
            oss << "PLUS";
            break;
        case OperatorType::DB_NOT:
            oss << "NOT";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", operand=";
    if (operand_) {
        oss << operand_->debug_string();
    } else {
        oss << "<null>";
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
