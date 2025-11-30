#include "dreamdb/parser/ast/unary_expr.h"

#include <sstream>

namespace dreamdb
{

UnaryExpr::UnaryExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::UNARY_EXPR, line, column)
    , op_type(UnaryOperatorType::NOT)
    , operand(nullptr)
{
}

void UnaryExpr::set_op_type(UnaryOperatorType op_type) noexcept
{
    this->op_type = op_type;
}

UnaryOperatorType UnaryExpr::get_op_type() const noexcept
{
    return op_type;
}

void UnaryExpr::set_operand(std::unique_ptr<AstNode> operand)
{
    this->operand = std::move(operand);
}

const AstNode * UnaryExpr::get_operand() const noexcept
{
    return operand.get();
}

AstNode * UnaryExpr::get_operand() noexcept
{
    return operand.get();
}

std::string UnaryExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "UnaryExpr(op=";
    
    switch (op_type) {
        case UnaryOperatorType::MINUS:
            oss << "MINUS";
            break;
        case UnaryOperatorType::PLUS:
            oss << "PLUS";
            break;
        case UnaryOperatorType::NOT:
            oss << "NOT";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }
    
    oss << ", operand=";
    if (operand) {
        oss << operand->debug_string();
    } else {
        oss << "<null>";
    }
    
    oss << ")";
    return oss.str();
}

} // namespace dreamdb
