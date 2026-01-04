#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

/**
 * @brief DROP 类型枚举
 */
enum class AstDropType : std::uint8_t
{
    AST_DROP_UNKNOWN,     // 未知
    AST_DROP_DATABASE,    // 数据库
    AST_DROP_COLLECTION,  // 集合
    AST_DROP_INDEX,       // 索引
    AST_DROP_VINDEX,      // 向量索引
};

/**
 * @brief 删除数据库操作
 */
class AstDropDatabase
{
public:
    AstDropDatabase(const std::string & database_name);

    AstDropDatabase(const AstDropDatabase &) = delete;

    AstDropDatabase(AstDropDatabase &&) noexcept = default;

    AstDropDatabase & operator=(const AstDropDatabase &) = delete;

    AstDropDatabase & operator=(AstDropDatabase &&) noexcept = default;

    ~AstDropDatabase() noexcept = default;

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
 * @brief 删除集合操作
 */
class AstDropCollection
{
public:
    AstDropCollection(const std::string & collection_name);

    AstDropCollection(const AstDropCollection &) = delete;

    AstDropCollection(AstDropCollection &&) noexcept = default;

    AstDropCollection & operator=(const AstDropCollection &) = delete;

    AstDropCollection & operator=(AstDropCollection &&) noexcept = default;

    ~AstDropCollection() noexcept = default;

public:
    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

private:
    std::string collection_name_;  // 集合名称
};

/**
 * @brief 删除索引操作
 */
class AstDropIndex
{
public:
    AstDropIndex(const std::string & index_name, const std::string & collection_name);

    AstDropIndex(const AstDropIndex &) = delete;

    AstDropIndex(AstDropIndex &&) noexcept = default;

    AstDropIndex & operator=(const AstDropIndex &) = delete;

    AstDropIndex & operator=(AstDropIndex &&) noexcept = default;

    ~AstDropIndex() noexcept = default;

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

private:
    std::string index_name_;       // 索引名称
    std::string collection_name_;  // 集合名称
};

/**
 * @brief 删除向量索引操作
 */
class AstDropVIndex
{
public:
    AstDropVIndex(const std::string & vindex_name, const std::string & collection_name);

    AstDropVIndex(const AstDropVIndex &) = delete;

    AstDropVIndex(AstDropVIndex &&) noexcept = default;

    AstDropVIndex & operator=(const AstDropVIndex &) = delete;

    AstDropVIndex & operator=(AstDropVIndex &&) noexcept = default;

    ~AstDropVIndex() noexcept = default;

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

private:
    std::string vindex_name_;      // 向量索引名称
    std::string collection_name_;  // 集合名称
};

/**
 * @brief DROP 语句节点
 */
class AstDropStatementNode : public AstStatementNode
{
public:
    AstDropStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstDropStatementNode(const AstDropStatementNode &) = delete;

    AstDropStatementNode(AstDropStatementNode &&) noexcept = default;

    AstDropStatementNode & operator=(const AstDropStatementNode &) = delete;

    AstDropStatementNode & operator=(AstDropStatementNode &&) noexcept = default;

    ~AstDropStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置 DROP 类型
     * @param drop_type DROP 类型
     */
    void set_drop_type(AstDropType drop_type) noexcept;

    /**
     * @brief 设置删除数据库操作
     * @param op 删除数据库操作
     */
    void set_drop_database(AstDropDatabase && op);

    /**
     * @brief 设置删除集合操作
     * @param op 删除集合操作
     */
    void set_drop_collection(AstDropCollection && op);

    /**
     * @brief 设置删除索引操作
     * @param op 删除索引操作
     */
    void set_drop_index(AstDropIndex && op);

    /**
     * @brief 设置删除向量索引操作
     * @param op 删除向量索引操作
     */
    void set_drop_vindex(AstDropVIndex && op);

    /**
     * @brief 获取 DROP 类型
     * @return DROP 类型
     */
    AstDropType get_drop_type() const noexcept;

    /**
     * @brief 获取删除数据库操作
     * @return 删除数据库操作
     */
    const AstDropDatabase & get_drop_database() const;

    /**
     * @brief 获取删除集合操作
     * @return 删除集合操作
     */
    const AstDropCollection & get_drop_collection() const;

    /**
     * @brief 获取删除索引操作
     * @return 删除索引操作
     */
    const AstDropIndex & get_drop_index() const;

    /**
     * @brief 获取删除向量索引操作
     * @return 删除向量索引操作
     */
    const AstDropVIndex & get_drop_vindex() const;

    /**
     * @brief 是否设置 DROP 类型
     * @return 是否设置 DROP 类型
     */
    bool has_drop_type() const noexcept;

    /**
     * @brief 是否存在删除操作
     * @return 是否存在删除操作
     */
    bool has_drop_operation() const noexcept;

    /**
     * @brief 是否存在删除数据库操作
     * @return 是否存在删除数据库操作
     */
    bool has_drop_database() const noexcept;

    /**
     * @brief 是否存在删除集合操作
     * @return 是否存在删除集合操作
     */
    bool has_drop_collection() const noexcept;

    /**
     * @brief 是否存在删除索引操作
     * @return 是否存在删除索引操作
     */
    bool has_drop_index() const noexcept;

    /**
     * @brief 是否存在删除向量索引操作
     * @return 是否存在删除向量索引操作
     */
    bool has_drop_vindex() const noexcept;

private:
    AstDropType drop_type_;                                           // DROP 类型
    std::variant<
        std::monostate,
        AstDropDatabase,
        AstDropCollection,
        AstDropIndex,
        AstDropVIndex
    > drop_operation_;                                                // 删除操作
};

} // namespace dreamdb
