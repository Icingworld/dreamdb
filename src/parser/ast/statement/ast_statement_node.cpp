#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

AstStatementNode::AstStatementNode(AstStatementNodeType statement_type, std::size_t line, std::size_t column) noexcept
    : AstNode(line, column)
    , statement_type_(statement_type)
{
}

AstStatementNodeType AstStatementNode::get_statement_type() const noexcept
{
    return statement_type_;
}

} // namespace dreamdb
