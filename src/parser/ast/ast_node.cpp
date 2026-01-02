#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

AstNode::AstNode(std::size_t line, std::size_t column) noexcept
    : line_(line)
    , column_(column)
{
}

std::size_t AstNode::get_line() const noexcept
{
    return line_;
}

std::size_t AstNode::get_column() const noexcept
{
    return column_;
}

} // namespace dreamdb
