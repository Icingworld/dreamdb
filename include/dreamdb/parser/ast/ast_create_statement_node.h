#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <variant>

#include "dreamdb/parser/ast/ast_statement_node.h"
#include "dreamdb/parser/ast/column_definition.h"
#include "dreamdb/common/type.h"


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

    VIndexWithClause(const VIndexWithClause &) = default;

    VIndexWithClause(VIndexWithClause &&) noexcept = default;

    VIndexWithClause & operator=(const VIndexWithClause &) = default;

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
 * @brief 创建数据库操作
 */
class AstCreateDatabase
{
public:
    AstCreateDatabase(const std::string & database_name);

    AstCreateDatabase(const AstCreateDatabase &) = delete;

    AstCreateDatabase(AstCreateDatabase &&) noexcept = default;

    AstCreateDatabase & operator=(const AstCreateDatabase &) = delete;

    AstCreateDatabase & operator=(AstCreateDatabase &&) noexcept = default;

    ~AstCreateDatabase() noexcept = default;

public:
    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & get_database_name() const noexcept;

private:
    std::string database_name_;  // 数据库名称
};

/**
 * @brief 创建集合操作
 */
class AstCreateCollection
{
public:
    AstCreateCollection(const std::string & collection_name, std::vector<ColumnDefinition> && column_definitions);

    AstCreateCollection(const AstCreateCollection &) = delete;

    AstCreateCollection(AstCreateCollection &&) noexcept = default;

    AstCreateCollection & operator=(const AstCreateCollection &) = delete;

    AstCreateCollection & operator=(AstCreateCollection &&) noexcept = default;

    ~AstCreateCollection() noexcept = default;

public:
    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取所有列定义
     * @return 列定义列表
     */
    const std::vector<ColumnDefinition> & get_column_definitions() const noexcept;

private:
    std::string collection_name_;                       // 集合名称
    std::vector<ColumnDefinition> column_definitions_;  // 列定义列表
};

/**
 * @brief 创建索引操作
 */
class AstCreateIndex
{
public:
    AstCreateIndex(
        const std::string & index_name,
        const std::string & collection_name,
        const std::vector<std::string> & column_names,
        IndexType index_type
    );

    AstCreateIndex(const AstCreateIndex &) = delete;

    AstCreateIndex(AstCreateIndex &&) noexcept = default;

    AstCreateIndex & operator=(const AstCreateIndex &) = delete;

    AstCreateIndex & operator=(AstCreateIndex &&) noexcept = default;

    ~AstCreateIndex() noexcept = default;

public:
    /**
     * @brief 获取索引名称
     * @return 索引名称
     */
    const std::string & get_index_name() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取列名列表
     * @return 列名列表
     */
    const std::vector<std::string> & get_column_names() const noexcept;

    /**
     * @brief 获取索引类型
     * @return 索引类型
     */
    IndexType get_index_type() const noexcept;

private:
    std::string index_name_;                    // 索引名称
    std::string collection_name_;               // 集合名称
    std::vector<std::string> column_names_;     // 列名列表
    IndexType index_type_;                      // 索引类型
};

/**
 * @brief 创建向量索引操作
 */
class AstCreateVIndex
{
public:
    AstCreateVIndex(
        const std::string & vindex_name,
        const std::string & collection_name,
        const std::vector<std::string> & column_names,
        VIndexType vindex_type,
        const VIndexWithClause & with_clause
    );

    AstCreateVIndex(const AstCreateVIndex &) = delete;

    AstCreateVIndex(AstCreateVIndex &&) noexcept = default;

    AstCreateVIndex & operator=(const AstCreateVIndex &) = delete;

    AstCreateVIndex & operator=(AstCreateVIndex &&) noexcept = default;

    ~AstCreateVIndex() noexcept = default;

public:
    /**
     * @brief 获取向量索引名称
     * @return 向量索引名称
     */
    const std::string & get_vindex_name() const noexcept;

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取列名列表
     * @return 列名列表
     */
    const std::vector<std::string> & get_column_names() const noexcept;

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

private:
    std::string vindex_name_;                   // 向量索引名称
    std::string collection_name_;               // 集合名称
    std::vector<std::string> column_names_;     // 列名列表
    VIndexType vindex_type_;                    // 向量索引类型
    VIndexWithClause vindex_with_clause_;       // 向量索引 WITH 子句
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
     * @brief 设置是否跳过存在性检查
     * @param is_if_not_exists 是否跳过存在性检查
     */
    void set_is_if_not_exists(bool is_if_not_exists) noexcept;

    /**
     * @brief 设置创建数据库操作
     * @param op 创建数据库操作
     */
    void set_create_database(AstCreateDatabase && op);

    /**
     * @brief 设置创建集合操作
     * @param op 创建集合操作
     */
    void set_create_collection(AstCreateCollection && op);

    /**
     * @brief 设置创建索引操作
     * @param op 创建索引操作
     */
    void set_create_index(AstCreateIndex && op);

    /**
     * @brief 设置创建向量索引操作
     * @param op 创建向量索引操作
     */
    void set_create_vindex(AstCreateVIndex && op);

    /**
     * @brief 获取对象类型
     * @return 对象类型
     */
    AstCreateType get_create_type() const noexcept;

    /**
     * @brief 获取是否跳过存在性检查
     * @return 是否跳过存在性检查
     */
    bool get_is_if_not_exists() const noexcept;

    /**
     * @brief 获取创建数据库操作
     * @return 创建数据库操作
     */
    const AstCreateDatabase & get_create_database() const;

    /**
     * @brief 获取创建集合操作
     * @return 创建集合操作
     */
    const AstCreateCollection & get_create_collection() const;

    /**
     * @brief 获取创建索引操作
     * @return 创建索引操作
     */
    const AstCreateIndex & get_create_index() const;

    /**
     * @brief 获取创建向量索引操作
     * @return 创建向量索引操作
     */
    const AstCreateVIndex & get_create_vindex() const;

    /**
     * @brief 是否设置 CREATE 类型
     * @return 是否设置 CREATE 类型
     */
    bool has_create_type() const noexcept;

    /**
     * @brief 是否存在创建操作
     * @return 是否存在创建操作
     */
    bool has_create_operation() const noexcept;

    /**
     * @brief 是否存在创建数据库操作
     * @return 是否存在创建数据库操作
     */
    bool has_create_database() const noexcept;

    /**
     * @brief 是否存在创建集合操作
     * @return 是否存在创建集合操作
     */
    bool has_create_collection() const noexcept;

    /**
     * @brief 是否存在创建索引操作
     * @return 是否存在创建索引操作
     */
    bool has_create_index() const noexcept;

    /**
     * @brief 是否存在创建向量索引操作
     * @return 是否存在创建向量索引操作
     */
    bool has_create_vindex() const noexcept;

private:
    AstCreateType create_type_;                                           // 创建类型
    bool is_if_not_exists_;                                               // 是否跳过存在性检查
    std::variant<
        std::monostate,
        AstCreateDatabase,
        AstCreateCollection,
        AstCreateIndex,
        AstCreateVIndex
    > create_operation_;                                                  // 创建操作
};

} // namespace dreamdb
