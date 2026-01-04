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

AstCreateDatabase::AstCreateDatabase(const std::string & database_name)
    : database_name_(database_name)
{
}

const std::string & AstCreateDatabase::get_database_name() const noexcept
{
    return database_name_;
}

AstCreateCollection::AstCreateCollection(const std::string & collection_name, std::vector<ColumnDefinition> && column_definitions)
    : collection_name_(collection_name)
    , column_definitions_(std::move(column_definitions))
{
}

const std::string & AstCreateCollection::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::vector<ColumnDefinition> & AstCreateCollection::get_column_definitions() const noexcept
{
    return column_definitions_;
}

AstCreateIndex::AstCreateIndex(const std::string & index_name, const std::string & collection_name, const std::vector<std::string> & column_names, IndexType index_type)
    : index_name_(index_name)
    , collection_name_(collection_name)
    , column_names_(column_names)
    , index_type_(index_type)
{
}

const std::string & AstCreateIndex::get_index_name() const noexcept
{
    return index_name_;
}

const std::string & AstCreateIndex::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::vector<std::string> & AstCreateIndex::get_column_names() const noexcept
{
    return column_names_;
}

IndexType AstCreateIndex::get_index_type() const noexcept
{
    return index_type_;
}

AstCreateVIndex::AstCreateVIndex(const std::string & vindex_name, const std::string & collection_name, const std::vector<std::string> & column_names, VIndexType vindex_type, const VIndexWithClause & with_clause)
    : vindex_name_(vindex_name)
    , collection_name_(collection_name)
    , column_names_(column_names)
    , vindex_type_(vindex_type)
    , vindex_with_clause_(with_clause)
{
}

const std::string & AstCreateVIndex::get_vindex_name() const noexcept
{
    return vindex_name_;
}

const std::string & AstCreateVIndex::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::vector<std::string> & AstCreateVIndex::get_column_names() const noexcept
{
    return column_names_;
}

VIndexType AstCreateVIndex::get_vindex_type() const noexcept
{
    return vindex_type_;
}

const VIndexWithClause & AstCreateVIndex::get_vindex_with_clause() const noexcept
{
    return vindex_with_clause_;
}

AstCreateStatementNode::AstCreateStatementNode(std::size_t line, std::size_t column)
    : AstStatementNode(AstStatementNodeType::AST_STATEMENT_CREATE, line, column)
    , create_type_(AstCreateType::AST_CREATE_UNKNOWN)
    , is_if_not_exists_(false)
    , create_operation_(std::monostate())
{
}

void AstCreateStatementNode::set_create_type(AstCreateType create_type) noexcept
{
    create_type_ = create_type;
}

void AstCreateStatementNode::set_is_if_not_exists(bool is_if_not_exists) noexcept
{
    is_if_not_exists_ = is_if_not_exists;
}

void AstCreateStatementNode::set_create_database(AstCreateDatabase && op)
{
    create_operation_ = std::move(op);
}

void AstCreateStatementNode::set_create_collection(AstCreateCollection && op)
{
    create_operation_ = std::move(op);
}

void AstCreateStatementNode::set_create_index(AstCreateIndex && op)
{
    create_operation_ = std::move(op);
}

void AstCreateStatementNode::set_create_vindex(AstCreateVIndex && op)
{
    create_operation_ = std::move(op);
}

AstCreateType AstCreateStatementNode::get_create_type() const noexcept
{
    return create_type_;
}

bool AstCreateStatementNode::get_is_if_not_exists() const noexcept
{
    return is_if_not_exists_;
}

const AstCreateDatabase & AstCreateStatementNode::get_create_database() const
{
    return std::get<AstCreateDatabase>(create_operation_);
}

const AstCreateCollection & AstCreateStatementNode::get_create_collection() const
{
    return std::get<AstCreateCollection>(create_operation_);
}

const AstCreateIndex & AstCreateStatementNode::get_create_index() const
{
    return std::get<AstCreateIndex>(create_operation_);
}

const AstCreateVIndex & AstCreateStatementNode::get_create_vindex() const
{
    return std::get<AstCreateVIndex>(create_operation_);
}

bool AstCreateStatementNode::has_create_type() const noexcept
{
    return create_type_ != AstCreateType::AST_CREATE_UNKNOWN;
}

bool AstCreateStatementNode::has_create_operation() const noexcept
{
    return !std::holds_alternative<std::monostate>(create_operation_);
}

bool AstCreateStatementNode::has_create_database() const noexcept
{
    return std::holds_alternative<AstCreateDatabase>(create_operation_);
}

bool AstCreateStatementNode::has_create_collection() const noexcept
{
    return std::holds_alternative<AstCreateCollection>(create_operation_);
}

bool AstCreateStatementNode::has_create_index() const noexcept
{
    return std::holds_alternative<AstCreateIndex>(create_operation_);
}

bool AstCreateStatementNode::has_create_vindex() const noexcept
{
    return std::holds_alternative<AstCreateVIndex>(create_operation_);
}

} // namespace dreamdb
