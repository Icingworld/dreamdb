#include "dreamdb/parser/ast/ast_node.h"

namespace dreamdb
{

AstNode::AstNode(AstNodeType type, std::size_t line, std::size_t column)
    : type(type)
    , line(line)
    , column(column)
{
}

AstNodeType AstNode::get_type() const noexcept
{
    return type;
}

std::size_t AstNode::get_line() const noexcept
{
    return line;
}

std::size_t AstNode::get_column() const noexcept
{
    return column;
}

void AstNode::set_source_location(std::size_t line, std::size_t column) noexcept
{
    this->line = line;
    this->column = column;
}

} // namespace dreamdb
