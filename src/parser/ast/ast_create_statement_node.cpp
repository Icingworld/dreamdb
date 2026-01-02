#include "dreamdb/parser/ast/ast_create_statement_node.h"

namespace dreamdb
{

VIndexWithClause::VIndexWithClause()
    : nlist(1024)
    , M(32)
    , ef_construction(200)
    , metric(MetricType::L2)
{
    // IVF_FLAT: nlist 建议为数据量的平方根，默认 1024 适合中规模数据量
    // HNSW: M 参数，大多数场景下性能更好，常见范围 4 - 64
    // HNSW: ef_construction 参数，有较好的召回率，常见范围 50 - 500
    // 距离度量方式，L2 距离更常用
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

AstCreateStatementNode::~AstCreateStatementNode() noexcept = default;

void AstCreateStatementNode::set_create_type(AstCreateType create_type) noexcept
{
    create_type_ = create_type;

    // 根据不同类型初始化 optional 成员
    switch (create_type_) {
        case AstCreateType::AST_CREATE_DATABASE:
            column_definitions_.clear();
            collection_name_ = std::nullopt;
            column_names_.clear();
            index_type_ = std::nullopt;
            vindex_type_ = std::nullopt;
            break;
        case AstCreateType::AST_CREATE_COLLECTION:
            column_definitions_.clear();
            collection_name_ = std::nullopt;   // 该集合名仅在 INDEX 中使用，COLLECTION 使用的是 object_name_
            column_names_.clear();
            index_type_ = IndexType::BTREE;    // 默认使用 B-Tree 索引
            vindex_type_ = std::nullopt;
            break;
        case AstCreateType::AST_CREATE_INDEX:
            column_definitions_.clear();
            collection_name_ = std::nullopt;   // 这里保持 nullopt，调用 setter 时设置
            column_names_.clear();
            index_type_ = IndexType::BTREE;    // 默认使用 B-Tree 索引
            vindex_type_ = std::nullopt;
            break;
        case AstCreateType::AST_CREATE_VINDEX:
            column_definitions_.clear();
            collection_name_ = std::nullopt;
            column_names_.clear();
            index_type_ = std::nullopt;
            vindex_type_ = VIndexType::FLAT;   // 默认使用线性扫描索引
            break;
        default:
            break;
    }
}

void AstCreateStatementNode::set_object_name(const std::string & name)
{
    object_name_ = name;
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
    collection_name_ = collection_name;
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

const std::string & AstCreateStatementNode::get_object_name() const noexcept
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

const std::string & AstCreateStatementNode::get_collection_name() const noexcept
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
