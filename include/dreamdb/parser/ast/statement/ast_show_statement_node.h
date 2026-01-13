#pragma once

#include <cstddef>
#include <string>
#include <variant>
#include <optional>

#include "dreamdb/parser/ast/ast_statement_node.h"

namespace dreamdb
{

/**
 * @brief 显示数据库操作
 */
class AstShowDatabases
{
public:
    AstShowDatabases() = default;

    AstShowDatabases(const AstShowDatabases &) = delete;

    AstShowDatabases(AstShowDatabases &&) noexcept = default;

    AstShowDatabases & operator=(const AstShowDatabases &) = delete;

    AstShowDatabases & operator=(AstShowDatabases &&) noexcept = default;

    ~AstShowDatabases() noexcept = default;
};

/**
 * @brief 显示集合操作
 */
class AstShowCollections
{
public:
    AstShowCollections();

    AstShowCollections(const AstShowCollections &) = delete;

    AstShowCollections(AstShowCollections &&) noexcept = default;

    AstShowCollections & operator=(const AstShowCollections &) = delete;

    AstShowCollections & operator=(AstShowCollections &&) noexcept = default;

    ~AstShowCollections() noexcept = default;

public:
    /**
     * @brief 设置数据库名称
     * @param database_name 数据库名称
     */
    void set_database_name(const std::string & database_name);

    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & get_database_name() const;

    /**
     * @brief 是否存在数据库名称
     * @return 是否存在数据库名称
     */
    bool has_database_name() const noexcept;

private:
    std::optional<std::string> database_name_;  // 数据库名称
};

/**
 * @brief 显示索引操作
 */
class AstShowIndexes
{
public:
    AstShowIndexes(const std::string & collection_name);

    AstShowIndexes(const AstShowIndexes &) = delete;

    AstShowIndexes(AstShowIndexes &&) noexcept = default;

    AstShowIndexes & operator=(const AstShowIndexes &) = delete;

    AstShowIndexes & operator=(AstShowIndexes &&) noexcept = default;

    ~AstShowIndexes() noexcept = default;

public:
    /**
     * @brief 设置数据库名称
     * @param database_name 数据库名称
     */
    void set_database_name(const std::string & database_name);

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & get_database_name() const;

    /**
     * @brief 是否存在数据库名称
     * @return 是否存在数据库名称
     */
    bool has_database_name() const noexcept;

private:
    std::string collection_name_;               // 集合名称
    std::optional<std::string> database_name_;  // 数据库名称
};

/**
 * @brief 显示向量索引操作
 */
class AstShowVIndexes
{
public:
    AstShowVIndexes(const std::string & collection_name);

    AstShowVIndexes(const AstShowVIndexes &) = delete;

    AstShowVIndexes(AstShowVIndexes &&) noexcept = default;

    AstShowVIndexes & operator=(const AstShowVIndexes &) = delete;

    AstShowVIndexes & operator=(AstShowVIndexes &&) noexcept = default;

    ~AstShowVIndexes() noexcept = default;

public:
    /**
     * @brief 设置数据库名称
     * @param database_name 数据库名称
     */
    void set_database_name(const std::string & database_name);

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_collection_name() const noexcept;

    /**
     * @brief 获取数据库名称
     * @return 数据库名称
     */
    const std::string & get_database_name() const;

    /**
     * @brief 是否存在数据库名称
     * @return 是否存在数据库名称
     */
    bool has_database_name() const noexcept;

private:
    std::string collection_name_;               // 集合名称
    std::optional<std::string> database_name_;  // 数据库名称
};

/**
 * @brief SHOW 语句节点
 */
class AstShowStatementNode : public AstStatementNode
{
public:
    AstShowStatementNode(std::size_t line = 0, std::size_t column = 0);

    AstShowStatementNode(const AstShowStatementNode &) = delete;

    AstShowStatementNode(AstShowStatementNode &&) noexcept = default;

    AstShowStatementNode & operator=(const AstShowStatementNode &) = delete;

    AstShowStatementNode & operator=(AstShowStatementNode &&) noexcept = default;

    ~AstShowStatementNode() noexcept override = default;

public:
    /**
     * @brief 设置显示数据库操作
     * @param op 显示数据库操作
     */
    void set_show_databases(AstShowDatabases && op);

    /**
     * @brief 设置显示集合操作
     * @param op 显示集合操作
     */
    void set_show_collections(AstShowCollections && op);

    /**
     * @brief 设置显示索引操作
     * @param op 显示索引操作
     */
    void set_show_indexes(AstShowIndexes && op);

    /**
     * @brief 设置显示向量索引操作
     * @param op 显示向量索引操作
     */
    void set_show_vindexes(AstShowVIndexes && op);

    /**
     * @brief 获取显示数据库操作
     * @return 显示数据库操作
     */
    const AstShowDatabases & get_show_databases() const;

    /**
     * @brief 获取显示集合操作
     * @return 显示集合操作
     */
    const AstShowCollections & get_show_collections() const;

    /**
     * @brief 获取显示索引操作
     * @return 显示索引操作
     */
    const AstShowIndexes & get_show_indexes() const;

    /**
     * @brief 获取显示向量索引操作
     * @return 显示向量索引操作
     */
    const AstShowVIndexes & get_show_vindexes() const;

    /**
     * @brief 是否存在显示操作
     * @return 是否存在显示操作
     */
    bool has_show_operation() const noexcept;

    /**
     * @brief 是否存在显示数据库操作
     * @return 是否存在显示数据库操作
     */
    bool has_show_databases() const noexcept;

    /**
     * @brief 是否存在显示集合操作
     * @return 是否存在显示集合操作
     */
    bool has_show_collections() const noexcept;

    /**
     * @brief 是否存在显示索引操作
     * @return 是否存在显示索引操作
     */
    bool has_show_indexes() const noexcept;

    /**
     * @brief 是否存在显示向量索引操作
     * @return 是否存在显示向量索引操作
     */
    bool has_show_vindexes() const noexcept;

private:
    std::variant<
        std::monostate,
        AstShowDatabases,
        AstShowCollections,
        AstShowIndexes,
        AstShowVIndexes
    > show_operation_;                                                // 显示操作
};

} // namespace dreamdb
