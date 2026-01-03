#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"
#include "dreamdb/parser/ast/column_definition.h"


namespace dreamdb
{

/**
 * @brief 对象类型枚举
 */
enum class AstCreateType : std::uint8_t
{
    AST_CREATE_UNKNOWN,     // 未知
    AST_CREATE_DATABASE,    // 数据库
    AST_CREATE_COLLECTION,  // 集合
    AST_CREATE_INDEX,       // 索引
    AST_CREATE_VINDEX       // 向量索引
};

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

    ~VIndexWithClause() noexcept = default;

public:
    /**
     * @brief 设置 nlist - 聚类中心数量
     * @param nlist 聚类中心数量，控制向量分簇数
     */
    void set_nlist(std::int32_t nlist) noexcept;

    /**
     * @brief 设置 M - 节点在图中连接的最大邻居数
     * @param M 节点在图中连接的最大邻居数，控制图的连接密度
     */
    void set_M(std::int32_t M) noexcept;

    /**
     * @brief 设置 ef_construction - 每个新节点需要连接的候选邻居数量
     * @param ef_construction 每个新节点需要连接的候选邻居数量，控制索引构建的精度
     */
    void set_ef_construction(std::int32_t ef_construction) noexcept;

    /**
     * @brief 设置距离度量方式
     * @param metric 距离度量方式
     */
    void set_metric(MetricType metric) noexcept;

    /**
     * @brief 获取 nlist
     * @return nlist
     */
    std::int32_t get_nlist() const noexcept;

    /**
     * @brief 获取 M
     * @return M
     */
    std::int32_t get_M() const noexcept;

    /**
     * @brief 获取 ef_construction
     * @return ef_construction
     */
    std::int32_t get_ef_construction() const noexcept;

    /**
     * @brief 获取距离度量方式
     * @return 距离度量方式
     */
    MetricType get_metric() const noexcept;

private:
    std::int32_t nlist_;              //  聚类中心数量，IVF_FLAT 参数
    std::int32_t M_;                  //  节点在图中连接的最大邻居数，HNSW 参数
    std::int32_t ef_construction_;    //  每个新节点需要连接的候选邻居数量，HNSW 参数
    MetricType metric_;               //  距离度量方式
};

/**
 * @brief CREATE 语句节点
 */
class AstCreateStatementNode : public AstStatementNode
{
public:
    AstCreateStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstCreateStatementNode(const AstCreateStatementNode &) = delete;

    AstCreateStatementNode(AstCreateStatementNode &&) noexcept = default;

    AstCreateStatementNode & operator=(const AstCreateStatementNode &) = delete;

    AstCreateStatementNode & operator=(AstCreateStatementNode &&) noexcept = default;

    ~AstCreateStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置对象类型
     * @param type 对象类型
     */
    void set_create_type(AstCreateType create_type) noexcept;

    /**
     * @brief 设置对象名称
     * @param object_name 对象名称
     */
    void set_object_name(const std::string & object_name);

    /**
     * @brief 设置是否跳过存在性检查
     * @param is_if_not_exists 是否跳过存在性检查
     */
    void set_is_if_not_exists(bool is_if_not_exists) noexcept;

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
    void set_index_type(IndexType index_type) noexcept;

    /**
     * @brief 设置向量索引类型
     * @param vindex_type 向量索引类型
     */
    void set_vindex_type(VIndexType vindex_type) noexcept;

    /**
     * @brief 设置向量索引 WITH 子句
     * @param with_clause 向量索引 WITH 子句
     */
    void set_vindex_with_clause(VIndexWithClause && with_clause);

    /**
     * @brief 获取对象类型
     * @return 对象类型
     */
    AstCreateType get_create_type() const noexcept;

    /**
     * @brief 获取对象名称
     * @return 对象名称
     */
    const std::string & get_object_name() const;

    /**
     * @brief 获取是否跳过存在性检查
     * @return 是否跳过存在性检查
     */
    bool get_is_if_not_exists() const noexcept;

    /**
     * @brief 获取所有列定义
     * @return 列定义列表
     */
    const std::vector<ColumnDefinition> & get_column_definitions() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const;

    /**
     * @brief 获取列名列表
     * @return 列名列表
     */
    const std::vector<std::string> & get_column_names() const noexcept;

    /**
     * @brief 获取标量索引类型
     * @return 标量索引类型
     */
    IndexType get_index_type() const noexcept;

    /**
     * @brief 获取向量索引类型
     * @return 向量索引类型
     */
    VIndexType get_vindex_type() const noexcept;

    /**
     * @brief 获取向量索引 WITH 子句
     * @return 向量索引 WITH 子句
     */
    const VIndexWithClause & get_vindex_with_clause() const noexcept;

    /**
     * @brief 是否存在对象名称
     * @return 是否存在对象名称
     */
    bool has_object_name() const noexcept;

    /**
     * @brief 是否存在列定义列表
     * @return 是否存在列定义列表
     */
    bool has_column_definitions() const noexcept;

    /**
     * @brief 是否存在集合名称
     * @return 是否存在集合名称
     */
    bool has_collection_name() const noexcept;

    /**
     * @brief 是否存在列名列表
     * @return 是否存在列名列表
     */
    bool has_column_names() const noexcept;

    /**
     * @brief 是否存在标量索引类型
     * @return 是否存在标量索引类型
     */
    bool has_index_type() const noexcept;

    /**
     * @brief 是否存在向量索引类型
     * @return 是否存在向量索引类型
     */
    bool has_vindex_type() const noexcept;

    /**
     * @brief 是否存在向量索引 WITH 子句
     * @return 是否存在向量索引 WITH 子句
     */
    bool has_vindex_with_clause() const noexcept;

private:
    AstCreateType create_type_;                                           // 创建类型
    std::optional<std::string> object_name_;                              // 对象名称
    bool is_if_not_exists_;                                               // 是否跳过存在性检查
    std::vector<ColumnDefinition> column_definitions_;                    // 列定义列表，用于创建集合
    std::optional<std::string> collection_name_;                          // 集合名称，用于创建集合
    std::vector<std::string> column_names_;                               // 列名列表，用于创建集合
    std::optional<IndexType> index_type_;                                 // 标量索引类型，用于创建索引
    std::optional<VIndexType> vindex_type_;                               // 向量索引类型，用于创建向量索引
    std::optional<VIndexWithClause> vindex_with_clause_;                  // 向量索引 WITH 子句，用于创建向量索引
};

} // namespace dreamdb
