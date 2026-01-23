#include "dreamdb/parser/ast/statement/select.h"

#include <cassert>

#include "dreamdb/parser/ast/expression/expression.h"
#include "dreamdb/parser/ast/statement/visitor.h"

namespace dreamdb::parser::ast
{

AstSelectExpressionItem::AstSelectExpressionItem(std::unique_ptr<AstExpression> expression, std::optional<std::string> alias)
    : expression(std::move(expression))
    , alias(std::move(alias))
{
    // 表达式不允许为空
    assert(this->expression != nullptr);
}

AstOrderByItem::AstOrderByItem(std::unique_ptr<AstExpression> expression, common::Direction direction)
    : expression(std::move(expression))
    , direction(direction)
{
    // 表达式不允许为空
    assert(this->expression != nullptr);
}

AstSelectStatement::AstSelectStatement(
    std::string collection_name,
    std::vector<AstSelectItem> select_items,
    std::unique_ptr<AstExpression> where,
    std::vector<std::unique_ptr<AstExpression>> group_by,
    std::unique_ptr<AstExpression> having,
    std::vector<AstOrderByItem> order_by,
    std::optional<std::uint64_t> limit,
    std::optional<std::uint64_t> offset,
    std::size_t line,
    std::size_t column
)
    : AstStatement(AstStatementType::Select, line, column)
    , collection_name_(std::move(collection_name))
    , select_items_(std::move(select_items))
    , where_(std::move(where))
    , group_by_(std::move(group_by))
    , having_(std::move(having))
    , order_by_items_(std::move(order_by))
    , limit_(limit)
    , offset_(offset)
{
    // 集合名称不能为空
    assert(!collection_name_.empty());

    // SELECT 项列表不能为空
    assert(!select_items_.empty());

    // GROUP BY 列表中的每个表达式都不能为空
    for (const auto & expr : group_by_) {
        assert(expr != nullptr);
    }
}

AstSelectStatement::~AstSelectStatement() noexcept = default;

std::unique_ptr<AstSelectStatement> AstSelectStatement::create(
    std::string collection_name,
    std::vector<AstSelectItem> select_items,
    std::unique_ptr<AstExpression> where,
    std::vector<std::unique_ptr<AstExpression>> group_by,
    std::unique_ptr<AstExpression> having,
    std::vector<AstOrderByItem> order_by,
    std::optional<std::uint64_t> limit,
    std::optional<std::uint64_t> offset,
    std::size_t line,
    std::size_t column
)
{
    return std::make_unique<AstSelectStatement>(
        std::move(collection_name),
        std::move(select_items),
        std::move(where),
        std::move(group_by),
        std::move(having),
        std::move(order_by),
        limit,
        offset,
        line,
        column
    );
}

const std::string & AstSelectStatement::collection_name() const noexcept
{
    return collection_name_;
}

const AstSelectItem & AstSelectStatement::select_item_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < select_items_.size());

    return select_items_[index];
}

std::size_t AstSelectStatement::select_item_count() const noexcept
{
    return select_items_.size();
}

bool AstSelectStatement::has_where() const noexcept
{
    return where_ != nullptr;
}

const AstExpression * AstSelectStatement::where() const noexcept
{
    return where_.get();
}

const AstExpression & AstSelectStatement::where_ref() const noexcept
{
    // WHERE 子句必须存在
    assert(where_ != nullptr);

    return *where_;
}

const AstExpression & AstSelectStatement::group_by_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < group_by_.size());

    return *group_by_[index];
}

std::size_t AstSelectStatement::group_by_count() const noexcept
{
    return group_by_.size();
}

bool AstSelectStatement::has_having() const noexcept
{
    return having_ != nullptr;
}

const AstExpression * AstSelectStatement::having() const noexcept
{
    return having_.get();
}

const AstExpression & AstSelectStatement::having_ref() const noexcept
{
    // HAVING 子句必须存在
    assert(having_ != nullptr);

    return *having_;
}

const AstOrderByItem & AstSelectStatement::order_by_item_at(std::size_t index) const noexcept
{
    // 索引不能超出范围
    assert(index < order_by_items_.size());

    return order_by_items_[index];
}

std::size_t AstSelectStatement::order_by_item_count() const noexcept
{
    return order_by_items_.size();
}

const std::optional<std::uint64_t> & AstSelectStatement::limit() const noexcept
{
    return limit_;
}

const std::optional<std::uint64_t> & AstSelectStatement::offset() const noexcept
{
    return offset_;
}

void AstSelectStatement::accept(AstStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

} // namespace dreamdb::parser::ast
