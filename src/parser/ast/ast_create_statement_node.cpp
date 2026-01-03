#include "dreamdb/parser/ast/ast_create_statement_node.h"

namespace dreamdb
{

VIndexWithClause::VIndexWithClause()
    : nlist_(1024)
    , M_(32)
    , ef_construction_(200)
    , metric_(MetricType::L2)
{
    // IVF_FLAT: nlist 建议为数据量的平方根，默认 1024 适合中规模数据量
    // HNSW: M 参数，大多数场景下性能更好，常见范围 4 - 64
    // HNSW: ef_construction 参数，有较好的召回率，常见范围 50 - 500
    // 距离度量方式，L2 距离更常用
}

void VIndexWithClause::set_nlist(std::int32_t nlist) noexcept
{
    nlist_ = nlist;
}

void VIndexWithClause::set_M(std::int32_t M) noexcept
{
    M_ = M;
}

void VIndexWithClause::set_ef_construction(std::int32_t ef_construction) noexcept
{
    ef_construction_ = ef_construction;
}

void VIndexWithClause::set_metric(MetricType metric) noexcept
{
    metric_ = metric;
}

std::int32_t VIndexWithClause::get_nlist() const noexcept
{
    return nlist_;
}

std::int32_t VIndexWithClause::get_M() const noexcept
{
    return M_;
}

std::int32_t VIndexWithClause::get_ef_construction() const noexcept
{
    return ef_construction_;
}

MetricType VIndexWithClause::get_metric() const noexcept
{
    return metric_;
}

AstCreateStatementNode::AstCreateStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_CREATE, line, column)
    , create_type_(AstCreateType::AST_CREATE_UNKNOWN)
    , object_name_(std::nullopt)
    , is_if_not_exists_(false)
    , column_definitions_()
    , collection_name_(std::nullopt)
    , column_names_()
    , index_type_(std::nullopt)
    , vindex_type_(std::nullopt)
    , vindex_with_clause_(std::nullopt)
{
}

void AstCreateStatementNode::set_create_type(AstCreateType create_type) noexcept
{
    create_type_ = create_type;
}

void AstCreateStatementNode::set_object_name(const std::string & object_name)
{
    if (object_name.empty()) {
        object_name_ = std::nullopt;
    } else {
        object_name_ = object_name;
    }
}

void AstCreateStatementNode::set_is_if_not_exists(bool is_if_not_exists) noexcept
{
    is_if_not_exists_ = is_if_not_exists;
}

void AstCreateStatementNode::add_column_definition(ColumnDefinition && column)
{
    column_definitions_.emplace_back(std::move(column));
}

void AstCreateStatementNode::set_collection_name(const std::string & collection_name)
{
    if (collection_name.empty()) {
        collection_name_ = std::nullopt;
    } else {
        collection_name_ = collection_name;
    }
}

void AstCreateStatementNode::add_column_name(const std::string & column_name)
{
    column_names_.emplace_back(column_name);
}

void AstCreateStatementNode::set_index_type(IndexType index_type) noexcept
{
    index_type_ = index_type;
}

void AstCreateStatementNode::set_vindex_type(VIndexType vindex_type) noexcept
{
    vindex_type_ = vindex_type;
}

void AstCreateStatementNode::set_vindex_with_clause(VIndexWithClause && with_clause)
{
    vindex_with_clause_ = std::move(with_clause);
}

AstCreateType AstCreateStatementNode::get_create_type() const noexcept
{
    return create_type_;
}

const std::string & AstCreateStatementNode::get_object_name() const
{
    return object_name_.value();
}

bool AstCreateStatementNode::get_is_if_not_exists() const noexcept
{
    return is_if_not_exists_;
}

const std::vector<ColumnDefinition> & AstCreateStatementNode::get_column_definitions() const noexcept
{
    return column_definitions_;
}

const std::string & AstCreateStatementNode::get_collection_name() const
{
    return collection_name_.value();
}

const std::vector<std::string> & AstCreateStatementNode::get_column_names() const noexcept
{
    return column_names_;
}

IndexType AstCreateStatementNode::get_index_type() const noexcept
{
    return index_type_.value();
}

VIndexType AstCreateStatementNode::get_vindex_type() const noexcept
{
    return vindex_type_.value();
}

const VIndexWithClause & AstCreateStatementNode::get_vindex_with_clause() const noexcept
{
    return vindex_with_clause_.value();
}

bool AstCreateStatementNode::has_object_name() const noexcept
{
    return object_name_.has_value();
}

bool AstCreateStatementNode::has_column_definitions() const noexcept
{
    return !column_definitions_.empty();
}

bool AstCreateStatementNode::has_collection_name() const noexcept
{
    return collection_name_.has_value();
}

bool AstCreateStatementNode::has_column_names() const noexcept
{
    return !column_names_.empty();
}

bool AstCreateStatementNode::has_index_type() const noexcept
{
    return index_type_.has_value();
}

bool AstCreateStatementNode::has_vindex_type() const noexcept
{
    return vindex_type_.has_value();
}

bool AstCreateStatementNode::has_vindex_with_clause() const noexcept
{
    return vindex_with_clause_.has_value();
}

} // namespace dreamdb
