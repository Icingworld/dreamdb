#include "dreamdb/parser/ast/binary_expr.h"

#include <sstream>

namespace dreamdb
{

BinaryExpr::BinaryExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::BINARY_EXPR, line, column)
    , op_type(BinaryOperatorType::PLUS)
    , left(nullptr)
    , right(nullptr)
{
}

void BinaryExpr::set_op_type(BinaryOperatorType op_type) noexcept
{
    this->op_type = op_type;
}

BinaryOperatorType BinaryExpr::get_op_type() const noexcept
{
    return op_type;
}

void BinaryExpr::set_left(std::unique_ptr<AstNode> left)
{
    this->left = std::move(left);
}

const AstNode * BinaryExpr::get_left() const noexcept
{
    return left.get();
}

AstNode * BinaryExpr::get_left() noexcept
{
    return left.get();
}

void BinaryExpr::set_right(std::unique_ptr<AstNode> right)
{
    this->right = std::move(right);
}

const AstNode * BinaryExpr::get_right() const noexcept
{
    return right.get();
}

AstNode * BinaryExpr::get_right() noexcept
{
    return right.get();
}

std::string BinaryExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "BinaryExpr(op=";
    
    switch (op_type) {
        case BinaryOperatorType::PLUS:
            oss << "PLUS";
            break;
        case BinaryOperatorType::MINUS:
            oss << "MINUS";
            break;
        case BinaryOperatorType::MULTIPLY:
            oss << "MULTIPLY";
            break;
        case BinaryOperatorType::DIVIDE:
            oss << "DIVIDE";
            break;
        case BinaryOperatorType::MODULO:
            oss << "MODULO";
            break;
        case BinaryOperatorType::EQUAL:
            oss << "EQUAL";
            break;
        case BinaryOperatorType::NOT_EQUAL:
            oss << "NOT_EQUAL";
            break;
        case BinaryOperatorType::LESS_THAN:
            oss << "LESS_THAN";
            break;
        case BinaryOperatorType::GREATER_THAN:
            oss << "GREATER_THAN";
            break;
        case BinaryOperatorType::LESS_EQUAL:
            oss << "LESS_EQUAL";
            break;
        case BinaryOperatorType::GREATER_EQUAL:
            oss << "GREATER_EQUAL";
            break;
        case BinaryOperatorType::AND:
            oss << "AND";
            break;
        case BinaryOperatorType::OR:
            oss << "OR";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }
    
    oss << ", left=";
    if (left) {
        oss << left->debug_string();
    } else {
        oss << "<null>";
    }
    
    oss << ", right=";
    if (right) {
        oss << right->debug_string();
    } else {
        oss << "<null>";
    }
    
    oss << ")";
    return oss.str();
}

} // namespace dreamdb
