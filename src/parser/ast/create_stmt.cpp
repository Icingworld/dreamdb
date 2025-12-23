#include "dreamdb/parser/ast/create_stmt.h"

#include <sstream>

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

std::string CreateStmt::create_type_to_string(CreateType create_type)
{
    switch (create_type) {
        case CreateType::DATABASE:
            return "DATABASE";
        case CreateType::COLLECTION:
            return "COLLECTION";
        case CreateType::INDEX:
            return "INDEX";
        case CreateType::VINDEX:
            return "VINDEX";
        default:
            return "UNKNOWN";
    }
}

CreateStmt::CreateStmt(std::size_t line, std::size_t column)
    : AstNode(AstNodeType::CREATE_STMT, line, column)
    , create_type_(CreateType::DATABASE)   // 默认 DATABASE 不需要用到列定义和集合名称
    , object_name_("")
    , is_if_not_exists_(false)
    , column_definitions_(std::nullopt)
    , collection_name_(std::nullopt)
    , column_names_(std::nullopt)
    , index_type_(std::nullopt)
    , vindex_type_(std::nullopt)
{
}

void CreateStmt::set_create_type(CreateType create_type)
{
    create_type_ = create_type;

    // 根据不同类型初始化 optional 成员
    switch (create_type_) {
        case CreateType::DATABASE:
            column_definitions_ = std::nullopt;
            collection_name_ = std::nullopt;
            column_names_ = std::nullopt;
            index_type_ = std::nullopt;
            vindex_type_ = std::nullopt;
            break;
        case CreateType::COLLECTION:
            column_definitions_ = std::vector<ColumnDefinition>();
            collection_name_ = std::nullopt;   // 该集合名仅在 INDEX 中使用，COLLECTION 使用的是 object_name_
            column_names_ = std::vector<std::string>();
            index_type_ = IndexType::BTREE;    // 默认使用 B-Tree 索引
            vindex_type_ = std::nullopt;
            break;
        case CreateType::INDEX:
            column_definitions_ = std::nullopt;
            collection_name_ = std::nullopt;   // 这里保持 nullopt，调用 setter 时设置
            column_names_ = std::vector<std::string>();
            index_type_ = IndexType::BTREE;    // 默认使用 B-Tree 索引
            vindex_type_ = std::nullopt;
            break;
        case CreateType::VINDEX:
            column_definitions_ = std::nullopt;
            collection_name_ = std::nullopt;
            column_names_ = std::vector<std::string>();
            index_type_ = std::nullopt;
            vindex_type_ = VIndexType::FLAT;   // 默认使用线性扫描索引
            break;
        default:
            break;
    }
}

void CreateStmt::set_object_name(const std::string & name)
{
    object_name_ = name;
}

void CreateStmt::set_is_if_not_exists(bool is_if_not_exists)
{
    is_if_not_exists_ = is_if_not_exists;
}

void CreateStmt::add_column_definition(ColumnDefinition && column)
{
    column_definitions_->emplace_back(std::move(column));
}

void CreateStmt::set_collection_name(const std::string & collection_name)
{
    collection_name_ = collection_name;
}

void CreateStmt::add_column_name(const std::string & column_name)
{
    column_names_->emplace_back(column_name);
}

void CreateStmt::set_index_type(IndexType index_type)
{
    index_type_ = index_type;
}

void CreateStmt::set_vindex_type(VIndexType vindex_type)
{
    vindex_type_ = vindex_type;
}

void CreateStmt::set_vindex_with_clause(VIndexWithClause && with_clause)
{
    with_clause_ = std::move(with_clause);
}

CreateStmt::CreateType CreateStmt::get_create_type() const noexcept
{
    return create_type_;
}

const std::string & CreateStmt::get_object_name() const noexcept
{
    return object_name_;
}

bool CreateStmt::get_is_if_not_exists() const noexcept
{
    return is_if_not_exists_;
}

const std::optional<std::vector<ColumnDefinition>> & CreateStmt::get_column_definitions() const noexcept
{
    return column_definitions_;
}

const std::optional<std::string> & CreateStmt::get_collection_name() const noexcept
{
    return collection_name_;
}

const std::optional<std::vector<std::string>> & CreateStmt::get_column_names() const noexcept
{
    return column_names_;
}

const std::optional<IndexType> & CreateStmt::get_index_type() const noexcept
{
    return index_type_;
}

const std::optional<VIndexType> & CreateStmt::get_vindex_type() const noexcept
{
    return vindex_type_;
}

const std::optional<VIndexWithClause> & CreateStmt::get_vindex_with_clause() const noexcept
{
    return with_clause_;
}

std::string CreateStmt::debug_string() const
{
    std::ostringstream oss;
    oss << "CreateStmt(";

    // 对象类型
    switch (create_type_) {
        case CreateType::DATABASE:
            oss << "DATABASE";
            break;
        case CreateType::COLLECTION:
            oss << "COLLECTION";
            break;
        case CreateType::INDEX:
            oss << "INDEX";
            break;
        default:
            oss << "UNKNOWN";
            break;
    }

    oss << ", name=" << (object_name_.empty() ? "<none>" : object_name_);

    // 是否跳过存在性检查
    if (is_if_not_exists_) {
        oss << ", if_not_exists=true";
    }

    // 列定义
    if (column_definitions_ && !column_definitions_->empty()) {
        oss << ", columns=[";
        for (std::size_t i = 0; i < column_definitions_->size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            const auto & col = column_definitions_->at(i);
            oss << col.get_name() << " ";

            // 类型
            switch (col.get_type()) {
                case FieldType::TINYINT:
                    oss << "TINYINT";
                    break;
                case FieldType::SMALLINT:
                    oss << "SMALLINT";
                    break;
                case FieldType::INTEGER:
                    oss << "INTEGER";
                    break;
                case FieldType::BIGINT:
                    oss << "BIGINT";
                    break;
                case FieldType::FLOAT:
                    oss << "FLOAT";
                    break;
                case FieldType::DOUBLE:
                    oss << "DOUBLE";
                    break;
                case FieldType::DECIMAL:
                    oss << "DECIMAL";
                    break;
                case FieldType::CHAR:
                    oss << "CHAR";
                    break;
                case FieldType::VARCHAR:
                    oss << "VARCHAR";
                    break;
                case FieldType::BOOLEAN:
                    oss << "BOOLEAN";
                    break;
                case FieldType::TIMESTAMP:
                    oss << "TIMESTAMP";
                    break;
                case FieldType::ENUM:
                    oss << "ENUM";
                    break;
                case FieldType::VECTOR:
                    oss << "VECTOR";
                    break;
                default:
                    oss << "UNKNOWN";
                    break;
            }

            // 长度
            if (col.get_length() > 0) {
                oss << "(" << col.get_length() << ")";
            }

            // 属性
            if (col.get_is_primary()) {
                oss << " PRIMARY_KEY";
            }
            if (col.get_is_auto_increment()) {
                oss << " AUTO_INCREMENT";
            }
            if (!col.get_is_nullable()) {
                oss << " NOT_NULL";
            }
        }
        oss << "]";
    }

    // 集合名称
    if (collection_name_ && !collection_name_->empty()) {
        oss << ", collection_name=" << *collection_name_;
    }

    oss << ")";

    return oss.str();
}

} // namespace dreamdb
