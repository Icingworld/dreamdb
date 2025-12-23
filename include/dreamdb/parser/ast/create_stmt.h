#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <optional>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/parser/ast/column_definition.h"


namespace dreamdb
{

/**
 * @brief 向量索引 WITH 子句
 * @details 表示向量索引 WITH 子句，用于指定向量索引的参数
 */
class VIndexWithClause
{
public:
    VIndexWithClause();

    VIndexWithClause(const VIndexWithClause &) = delete;

    VIndexWithClause(VIndexWithClause &&) noexcept = default;

    VIndexWithClause & operator=(const VIndexWithClause &) = delete;

    VIndexWithClause & operator=(VIndexWithClause &&) noexcept = default;

    ~VIndexWithClause() = default;

public:
    std::optional<std::int32_t> nlist;              // IVF_FLAT 参数
    std::optional<std::int32_t> M;                  // HNSW 参数
    std::optional<std::int32_t> ef_construction;    // HNSW 参数
    std::optional<MetricType> metric;               // 距离度量方式
};

/**
 * @brief CREATE 语句节点
 * @details 表示 CREATE {DATABASE | COLLECTION | INDEX} [IF NOT EXISTS] <object_name> [ON COLLECTION(<collection_name>) | (...)] 语句
 */
class CreateStmt : public AstNode
{
public:
    /**
     * @brief 对象类型枚举
     */
    enum class CreateType : std::uint8_t
    {
        DATABASE,    // 数据库
        COLLECTION,  // 集合
        INDEX,       // 索引
        VINDEX       // 向量索引
    };

    /**
     * @brief 将对象类型转换为字符串
     * @param create_type 对象类型
     * @return 字符串
     */
    static std::string create_type_to_string(CreateType create_type);

public:
    CreateStmt(std::size_t line = 0, std::size_t column = 0);

    CreateStmt(const CreateStmt &) = delete;

    CreateStmt(CreateStmt &&) noexcept = default;

    CreateStmt & operator=(const CreateStmt &) = delete;

    CreateStmt & operator=(CreateStmt &&) noexcept = default;

    ~CreateStmt() override = default;

public:
    /**
     * @brief 设置对象类型
     * @param type 对象类型
     */
    void set_create_type(CreateType type);

    /**
     * @brief 设置对象名称
     * @param name 对象名称
     */
    void set_object_name(const std::string & name);

    /**
     * @brief 设置是否跳过存在性检查
     * @param is_if_not_exists 是否跳过存在性检查
     */
    void set_is_if_not_exists(bool is_if_not_exists);

    /**
     * @brief 添加列定义
     * @param column 列定义
     */
    void add_column_definition(ColumnDefinition && column);

    /**
     * @brief 设置集合名称
     * @param collection_name 集合名称
     */
    void set_collection_name(const std::string & collection_name);

    /**
     * @brief 设置列名
     * @param column_name 列名
     */
    void add_column_name(const std::string & column_name);

    /**
     * @brief 设置标量索引类型
     * @param index_type 标量索引类型
     */
    void set_index_type(IndexType index_type);

    /**
     * @brief 设置向量索引类型
     * @param vindex_type 向量索引类型
     */
    void set_vindex_type(VIndexType vindex_type);

    /**
     * @brief 设置向量索引 WITH 子句
     * @param with_clause 向量索引 WITH 子句
     */
    void set_vindex_with_clause(VIndexWithClause && with_clause);

    /**
     * @brief 获取对象类型
     * @return 对象类型
     */
    CreateType get_create_type() const noexcept;

    /**
     * @brief 获取对象名称
     * @return 对象名称
     */
    const std::string & get_object_name() const noexcept;

    /**
     * @brief 获取是否跳过存在性检查
     * @return 是否跳过存在性检查
     */
    bool get_is_if_not_exists() const noexcept;

    /**
     * @brief 获取所有列定义
     * @return 列定义列表
     */
    const std::optional<std::vector<ColumnDefinition>> & get_column_definitions() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::optional<std::string> & get_collection_name() const noexcept;

    /**
     * @brief 获取列名列表
     * @return 列名列表
     */
    const std::optional<std::vector<std::string>> & get_column_names() const noexcept;

    /**
     * @brief 获取标量索引类型
     * @return 标量索引类型
     */
    const std::optional<IndexType> & get_index_type() const noexcept;

    /**
     * @brief 获取向量索引类型
     * @return 向量索引类型
     */
    const std::optional<VIndexType> & get_vindex_type() const noexcept;

    /**
     * @brief 获取向量索引 WITH 子句
     * @return 向量索引 WITH 子句
     */
    const std::optional<VIndexWithClause> & get_vindex_with_clause() const noexcept;

public:
    /**
     * @brief 调试字符串
     * @return 调试字符串
     */
    std::string debug_string() const override;

private:
    CreateType create_type_;                               // 创建类型
    std::string object_name_;                              // 对象名称
    bool is_if_not_exists_;                                // 是否跳过存在性检查
    std::optional<std::vector<ColumnDefinition>> column_definitions_;   // 列定义列表，用于 COLLECTION
    std::optional<std::string> collection_name_;           // 集合名称，用于索引
    std::optional<std::vector<std::string>> column_names_; // 列名列表，用于索引
    std::optional<IndexType> index_type_;                  // 标量索引类型，用于索引
    std::optional<VIndexType> vindex_type_;                // 向量索引类型，用于向量索引
    std::optional<VIndexWithClause> with_clause_;          // WITH 子句，用于向量索引
};

} // namespace dreamdb
