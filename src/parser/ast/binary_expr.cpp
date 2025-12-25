#include "dreamdb/parser/ast/binary_expr.h"

#include <sstream>

namespace dreamdb
{

BinaryExpr::BinaryExpr(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::BINARY_EXPR, line, column)
    , operator_type_(OperatorType::DB_PLUS)
    , left_(nullptr)
    , right_(nullptr)
{
}

void BinaryExpr::set_operator_type(OperatorType operator_type) noexcept
{
    operator_type_ = operator_type;
}

void BinaryExpr::set_left(std::unique_ptr<AstNode> left) noexcept
{
    left_ = std::move(left);
}

void BinaryExpr::set_right(std::unique_ptr<AstNode> right) noexcept
{
    right_ = std::move(right);
}

BinaryExpr::OperatorType BinaryExpr::get_operator_type() const noexcept
{
    return operator_type_;
}

const AstNode * BinaryExpr::get_left() const noexcept
{
    return left_.get();
}

const AstNode * BinaryExpr::get_right() const noexcept
{
    return right_.get();
}

std::string BinaryExpr::debug_string() const
{
    std::ostringstream oss;
    oss << "BinaryExpr(op=";
    
    switch (operator_type_) {
        case OperatorType::DB_PLUS:
            oss << "PLUS";
            break;
        case OperatorType::DB_MINUS:
            oss << "MINUS";
            break;
        case OperatorType::DB_MULTIPLY:
            oss << "MULTIPLY";
            break;
        case OperatorType::DB_DIVIDE:
            oss << "DIVIDE";
            break;
        case OperatorType::DB_MODULO:
            oss << "MODULO";
            break;
        case OperatorType::DB_EQUAL:
            oss << "EQUAL";
            break;
        case OperatorType::DB_NOT_EQUAL:
            oss << "NOT_EQUAL";
            break;
        case OperatorType::DB_LESS_THAN:
            oss << "LESS_THAN";
            break;
        case OperatorType::DB_GREATER_THAN:
            oss << "GREATER_THAN";
            break;
        case OperatorType::DB_LESS_EQUAL:
            oss << "LESS_EQUAL";
            break;
        case OperatorType::DB_GREATER_EQUAL:
            oss << "GREATER_EQUAL";
            break;
        case OperatorType::DB_AND:
            oss << "AND";
            break;
        case OperatorType::DB_OR:
            oss << "OR";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }
    
    oss << ", left=";
    if (left_) {
        oss << left_->debug_string();
    } else {
        oss << "<null>";
    }
    
    oss << ", right=";
    if (right_) {
        oss << right_->debug_string();
    } else {
        oss << "<null>";
    }
    
    oss << ")";
    return oss.str();
}

} // namespace dreamdb
