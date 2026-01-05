#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <variant>

#include "dreamdb/parser/ast/ast_statement_node.h"
#include "dreamdb/parser/ast/ast_column_definition.h"

namespace dreamdb
{

/**
 * @brief 向量索引 WITH 子句选项
 */
class AstVIndexWithOption
{
public:
    AstVIndexWithOption(const std::string & key, std::unique_ptr<AstExpressionNode> value);

    AstVIndexWithOption(const AstVIndexWithOption &) = delete;

    AstVIndexWithOption(AstVIndexWithOption &&) noexcept = default;

    AstVIndexWithOption & operator=(const AstVIndexWithOption &) = delete;

    AstVIndexWithOption & operator=(AstVIndexWithOption &&) noexcept = default;

    ~AstVIndexWithOption() noexcept = default;

public:
    /**
     * @brief 获取键
     * @return 键
     */
    const std::string & get_key() const noexcept;

    /**
     * @brief 获取值
     * @return 值
     */
    const std::unique_ptr<AstExpressionNode> & get_value() const noexcept;

    /**
     * @brief 是否存在值
     * @return 是否存在值
     */
    bool has_value() const noexcept;

private:
    std::string key_;                              // 键
    std::unique_ptr<AstExpressionNode> value_;     // 值
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
    AstCreateCollection(const std::string & collection_name, std::vector<AstColumnDefinition> && column_definitions);

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
    const std::vector<AstColumnDefinition> & get_column_definitions() const noexcept;

private:
    std::string collection_name_;                          // 集合名称
    std::vector<AstColumnDefinition> column_definitions_;  // 列定义列表
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
        const std::string & index_type
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
    const std::string & get_index_type() const noexcept;

private:
    std::string index_name_;                    // 索引名称
    std::string collection_name_;               // 集合名称
    std::vector<std::string> column_names_;     // 列名列表
    std::string index_type_;                    // 索引类型
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
        const std::string & column_name,
        const std::string & vindex_type,
        std::vector<AstVIndexWithOption> with_clauses
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
     * @brief 获取列名
     * @return 列名
     */
    const std::string & get_column_name() const noexcept;

    /**
     * @brief 获取向量索引类型
     * @return 向量索引类型
     */
    const std::string & get_vindex_type() const noexcept;

    /**
     * @brief 获取向量索引 WITH 子句
     * @return 向量索引 WITH 子句
     */
    const std::vector<AstVIndexWithOption> & get_with_clauses() const noexcept;

private:
    std::string vindex_name_;                        // 向量索引名称
    std::string collection_name_;                    // 集合名称
    std::string column_name_;                        // 列名
    std::string vindex_type_;                        // 向量索引类型
    std::vector<AstVIndexWithOption> with_clauses_;  // 向量索引 WITH 子句列表
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
     * @brief 设置是否跳过存在性检查
     * @param if_not_exists 是否跳过存在性检查
     */
    void set_if_not_exists(bool if_not_exists) noexcept;

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
     * @brief 获取是否跳过存在性检查
     * @return 是否跳过存在性检查
     */
    bool get_if_not_exists() const noexcept;

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
    bool if_not_exists_;                                               // 是否跳过存在性检查
    std::variant<
        std::monostate,
        AstCreateDatabase,
        AstCreateCollection,
        AstCreateIndex,
        AstCreateVIndex
    > create_operation_;                                               // 创建操作
};

} // namespace dreamdb
