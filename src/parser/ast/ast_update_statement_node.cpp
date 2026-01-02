#include "dreamdb/parser/ast/ast_update_statement_node.h"

namespace dreamdb
{

UpdateAssignment::UpdateAssignment(const std::string & column_name, std::unique_ptr<AstExpressionNode> value)
    : column_name_(std::nullopt)
    , value_(std::move(value))
{
    if (column_name.empty()) {
        column_name_ = std::nullopt;
    } else {
        column_name_ = column_name;
    }
}

UpdateAssignment::~UpdateAssignment() noexcept = default;

const std::string & UpdateAssignment::get_column_name() const noexcept
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
    , order_by_items_()
    , limit_(std::nullopt)
    , offset_(std::nullopt)
{
}

AstUpdateStatementNode::~AstUpdateStatementNode() noexcept = default;

void AstUpdateStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

void AstUpdateStatementNode::add_assignment(UpdateAssignment && assignment)
{
    assignments_.emplace_back(std::move(assignment));
}

void AstUpdateStatementNode::set_where_clause(std::unique_ptr<AstExpressionNode> where_clause) noexcept
{
    where_clause_ = std::move(where_clause);
}

void AstUpdateStatementNode::add_order_by_item(OrderByItem && order_by_item) noexcept
{
    order_by_items_.emplace_back(std::move(order_by_item));
}

void AstUpdateStatementNode::set_limit(std::size_t limit) noexcept
{
    limit_ = limit;
}

void AstUpdateStatementNode::set_offset(std::size_t offset) noexcept
{
    offset_ = offset;
}

const std::string & AstUpdateStatementNode::get_collection_name() const noexcept
{
    return collection_name_.value();
}

const std::vector<UpdateAssignment> & AstUpdateStatementNode::get_assignments() const noexcept
{
    return assignments_;
}

const AstExpressionNode & AstUpdateStatementNode::get_where_clause() const noexcept
{
    return *where_clause_;
}

const std::vector<OrderByItem> & AstUpdateStatementNode::get_order_by_items() const noexcept
{
    return order_by_items_;
}

std::size_t AstUpdateStatementNode::get_limit() const noexcept
{
    return limit_.value();
}

std::size_t AstUpdateStatementNode::get_offset() const noexcept
{
    return offset_.value();
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

bool AstUpdateStatementNode::has_order_by_items() const noexcept
{
    return !order_by_items_.empty();
}

bool AstUpdateStatementNode::has_limit() const noexcept
{
    return limit_.has_value();
}

bool AstUpdateStatementNode::has_offset() const noexcept
{
    return offset_.has_value();
}

} // namespace dreamdb
