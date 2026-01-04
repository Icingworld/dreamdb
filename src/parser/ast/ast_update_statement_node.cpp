#include "dreamdb/parser/ast/ast_update_statement_node.h"

#include "dreamdb/parser/ast/ast_expression_node.h"

namespace dreamdb
{

UpdateAssignment::UpdateAssignment(const std::string & column_name, std::unique_ptr<AstExpressionNode> value)
    : column_name_(column_name)
    , value_(std::move(value))
{
}

UpdateAssignment::~UpdateAssignment() noexcept = default;

const std::string & UpdateAssignment::get_column_name() const
{
    return column_name_.value();
}

const AstExpressionNode & UpdateAssignment::get_value() const noexcept
{
    return *value_;
}

bool UpdateAssignment::has_column_name() const noexcept
{
    return column_name_.has_value();
}

bool UpdateAssignment::has_value() const noexcept
{
    return value_ != nullptr;
}

AstUpdateStatementNode::AstUpdateStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_UPDATE, line, column)
    , collection_name_(std::nullopt)
    , assignments_()
    , where_clause_(nullptr)
{
}

AstUpdateStatementNode::~AstUpdateStatementNode() noexcept = default;

void AstUpdateStatementNode::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void AstUpdateStatementNode::add_assignment(UpdateAssignment && assignment)
{
    assignments_.emplace_back(std::move(assignment));
}

void AstUpdateStatementNode::set_where_clause(std::unique_ptr<AstExpressionNode> where_clause) noexcept
{
    where_clause_ = std::move(where_clause);
}

const std::string & AstUpdateStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

const std::vector<UpdateAssignment> & AstUpdateStatementNode::get_assignments() const noexcept
{
    return assignments_;
}

const AstExpressionNode & AstUpdateStatementNode::get_where_clause() const
{
    return *where_clause_;
}

bool AstUpdateStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstUpdateStatementNode::has_assignments() const noexcept
{
    return !assignments_.empty();
}

bool AstUpdateStatementNode::has_where_clause() const noexcept
{
    return where_clause_ != nullptr;
}

} // namespace dreamdb
