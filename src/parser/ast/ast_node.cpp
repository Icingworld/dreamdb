#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

AstNode::AstNode(AstNodeType type, std::size_t line, std::size_t column)
    : type_(type)
    , line_(line)
    , column_(column)
{
}

AstNodeType AstNode::get_type() const noexcept
{
    return type_;
}

std::size_t AstNode::get_line() const noexcept
{
    return line_;
}

std::size_t AstNode::get_column() const noexcept
{
    return column_;
}

void AstNode::set_source_location(std::size_t line, std::size_t column) noexcept
{
    line_ = line;
    column_ = column;
}

} // namespace dreamdb
