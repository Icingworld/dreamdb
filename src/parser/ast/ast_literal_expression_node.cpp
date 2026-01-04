#include "dreamdb/parser/ast/ast_literal_expression_node.h"

namespace dreamdb
{

AstLiteralExpressionNode::AstLiteralExpressionNode(std::size_t line, std::size_t column)
    : AstExpressionNode(AstExpressionNodeType::AST_EXPRESSION_LITERAL, line, column)
    , literal_type_(AstLiteralType::AST_LITERAL_NULL)
    , bool_value_(false)
    , int_value_(0)
    , float_value_(0.0)
    , string_value_()
{
}

std::unique_ptr<AstLiteralExpressionNode> AstLiteralExpressionNode::create_integer(std::int64_t value)
{
    // 构造函数已经被标注为 private，不能使用 make_unique 创建对象
    auto node = new AstLiteralExpressionNode();
    node->literal_type_ = AstLiteralType::AST_LITERAL_INTEGER;
    node->int_value_ = value;
    return std::unique_ptr<AstLiteralExpressionNode>(node);
}

std::unique_ptr<AstLiteralExpressionNode> AstLiteralExpressionNode::create_float(double value)
{
    auto node = new AstLiteralExpressionNode();
    node->literal_type_ = AstLiteralType::AST_LITERAL_FLOAT;
    node->float_value_ = value;
    return std::unique_ptr<AstLiteralExpressionNode>(node);
}

std::unique_ptr<AstLiteralExpressionNode> AstLiteralExpressionNode::create_string(const std::string & value)
{
    auto node = new AstLiteralExpressionNode();
    node->literal_type_ = AstLiteralType::AST_LITERAL_STRING;
    node->string_value_ = value;
    return std::unique_ptr<AstLiteralExpressionNode>(node);
}

std::unique_ptr<AstLiteralExpressionNode> AstLiteralExpressionNode::create_boolean(bool value)
{
    auto node = new AstLiteralExpressionNode();
    node->literal_type_ = AstLiteralType::AST_LITERAL_BOOLEAN;
    node->bool_value_ = value;
    return std::unique_ptr<AstLiteralExpressionNode>(node);
}

std::unique_ptr<AstLiteralExpressionNode> AstLiteralExpressionNode::create_null()
{
    auto node = new AstLiteralExpressionNode();
    node->literal_type_ = AstLiteralType::AST_LITERAL_NULL;
    return std::unique_ptr<AstLiteralExpressionNode>(node);
}

AstLiteralType AstLiteralExpressionNode::get_literal_type() const noexcept
{
    return literal_type_;
}

std::int64_t AstLiteralExpressionNode::get_integer() const
{
    return int_value_;
}

double AstLiteralExpressionNode::get_float() const
{
    return float_value_;
}

const std::string & AstLiteralExpressionNode::get_string() const
{
    return string_value_;
}

bool AstLiteralExpressionNode::get_boolean() const
{
    return bool_value_;
}

bool AstLiteralExpressionNode::is_integer() const noexcept
{
    return literal_type_ == AstLiteralType::AST_LITERAL_INTEGER;
}

bool AstLiteralExpressionNode::is_float() const noexcept
{
    return literal_type_ == AstLiteralType::AST_LITERAL_FLOAT;
}

bool AstLiteralExpressionNode::is_string() const noexcept
{
    return literal_type_ == AstLiteralType::AST_LITERAL_STRING;
}

bool AstLiteralExpressionNode::is_boolean() const noexcept
{
    return literal_type_ == AstLiteralType::AST_LITERAL_BOOLEAN;
}

bool AstLiteralExpressionNode::is_null() const noexcept
{
    return literal_type_ == AstLiteralType::AST_LITERAL_NULL;
}

} // namespace dreamdb
