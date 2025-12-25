#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <optional>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/schema/entity.h"
#include "dreamdb/schema/database_manager.h"

namespace dreamdb
{

// 向前声明
class SelectStmt;
class DeleteStmt;
class InsertStmt;
class UpdateStmt;
class CreateStmt;
class DropStmt;
class UseStmt;
class DescribeStmt;
class ShowStmt;
class AlterStmt;
class Database;
class Collection;

/**
 * @brief 执行器结果
 * 
 * 统一表示所有 SQL 语句的执行结果：
 * - SELECT: 返回查询结果（rows）和行数
 * - INSERT/UPDATE/DELETE: 返回受影响的行数（affected_count）
 * - CREATE/DROP: 返回成功/失败状态
 */
class ExecutorResult
{
public:
    ExecutorResult();

    ExecutorResult(const ExecutorResult &) = delete;

    ExecutorResult(ExecutorResult &&) noexcept = default;

    ExecutorResult & operator=(const ExecutorResult &) = delete;

    ExecutorResult & operator=(ExecutorResult &&) noexcept = default;

    ~ExecutorResult() = default;

public:
    /**
     * @brief 设置执行成功
     */
    void set_is_success(bool is_success) noexcept;
    
    /**
     * @brief 设置消息（用于成功或错误信息）
     * @param message 消息内容
     */
    void set_message(const std::string & message);

    /**
     * @brief 设置受影响的行数（用于 INSERT/UPDATE/DELETE）
     * @param count 受影响的行数
     */
    void set_affected_count(std::size_t count) noexcept;

    /**
     * @brief 添加查询结果行（用于 SELECT）
     * @param entity 实体
     */
    void add_row(Entity && entity);

    /**
     * @brief 检查执行是否成功
     * @return 如果成功返回 true
     */
    bool get_is_success() const noexcept;

    /**
     * @brief 获取消息
     * @return 消息内容
     */
    const std::string & get_message() const noexcept;

    /**
     * @brief 获取受影响的行数
     * @return 受影响的行数
     */
    std::size_t get_affected_count() const noexcept;

    /**
     * @brief 获取查询结果行数
     * @return 结果行数
     */
    std::size_t get_row_count() const noexcept;

    /**
     * @brief 获取查询结果（用于 SELECT）
     * @return 结果行的引用
     */
    const std::vector<Entity> & get_rows() const noexcept;

private:
    bool is_success_;                               // 执行是否成功
    std::string message_;                           // 成功或错误信息
    std::optional<std::size_t> affected_count_;     // 受影响的行数
    std::optional<std::vector<Entity>> rows_;       // 查询结果
};

/**
 * @brief 执行器
 */
class Executor
{
public:
    Executor(std::unique_ptr<DatabaseManager> database_manager);

    Executor(const Executor &) = delete;

    Executor(Executor &&) noexcept = default;

    Executor & operator=(const Executor &) = delete;

    Executor & operator=(Executor &&) noexcept = default;

    ~Executor();

public:
    /**
     * @brief 执行 SQL 语句
     * @param ast 抽象语法树节点
     * @return 执行结果
     */
    ExecutorResult execute(const AstNode & ast);

private:
    /**
     * @brief 执行 SELECT 语句
     * @param select_stmt 选择语句节点
     * @return 执行结果
     */
    ExecutorResult execute_select(const SelectStmt & select_stmt);

    /**
     * @brief 执行 DELETE 语句
     * @param delete_stmt 删除语句节点
     * @return 执行结果
     */
    ExecutorResult execute_delete(const DeleteStmt & delete_stmt);

    /**
     * @brief 执行 INSERT 语句
     * @param insert_stmt 插入语句节点
     * @return 执行结果
     */
    ExecutorResult execute_insert(const InsertStmt & insert_stmt);

    /**
     * @brief 执行 UPDATE 语句
     * @param update_stmt 更新语句节点
     * @return 执行结果
     */
    ExecutorResult execute_update(const UpdateStmt & update_stmt);

    /**
     * @brief 执行 CREATE 语句
     * @param create_stmt 创建语句节点
     * @return 执行结果
     */
    ExecutorResult execute_create(const CreateStmt & create_stmt);

    /**
     * @brief 执行 DROP 语句
     * @param drop_stmt 删除语句节点
     * @return 执行结果
     */
    ExecutorResult execute_drop(const DropStmt & drop_stmt);

    /**
     * @brief 执行 USE 语句
     * @param use_stmt 切换数据库语句节点
     * @return 执行结果
     */
    ExecutorResult execute_use(const UseStmt & use_stmt);

    /**
     * @brief 执行 DESCRIBE 语句
     * @param describe_stmt 描述语句节点
     * @return 执行结果
     */
    ExecutorResult execute_describe(const DescribeStmt & describe_stmt);

    /**
     * @brief 执行 SHOW 语句
     * @param show_stmt 显示语句节点
     * @return 执行结果
     */
    ExecutorResult execute_show(const ShowStmt & show_stmt);

    /**
     * @brief 执行 ALTER 语句
     * @param alter_stmt 修改语句节点
     * @return 执行结果
     */
    ExecutorResult execute_alter(const AlterStmt & alter_stmt);

    /** 辅助解析语句 */

    /**
     * @brief 执行 CREATE DATABASE 语句
     * @param create_stmt 创建语句节点
     * @return 执行结果
     */
    ExecutorResult execute_create_database(const CreateStmt & create_stmt);

    /**
     * @brief 执行 CREATE COLLECTION 语句
     * @param create_stmt 创建语句节点
     * @return 执行结果
     */
    ExecutorResult execute_create_collection(const CreateStmt & create_stmt);

    /**
     * @brief 执行 CREATE INDEX 语句
     * @param create_stmt 创建语句节点
     * @return 执行结果
     */
    ExecutorResult execute_create_index(const CreateStmt & create_stmt);

    /**
     * @brief 执行 DROP DATABASE 语句
     * @param drop_stmt 删除语句节点
     * @return 执行结果
     */
    ExecutorResult execute_drop_database(const DropStmt & drop_stmt);

    /**
     * @brief 执行 DROP COLLECTION 语句
     * @param drop_stmt 删除语句节点
     * @return 执行结果
     */
    ExecutorResult execute_drop_collection(const DropStmt & drop_stmt);

    /**
     * @brief 执行 DROP INDEX 语句
     * @param drop_stmt 删除语句节点
     * @return 执行结果
     */
    ExecutorResult execute_drop_index(const DropStmt & drop_stmt);

    /**
     * @brief 执行 SHOW DATABASES 语句
     * @param show_stmt 显示语句节点
     * @return 执行结果
     */
    ExecutorResult execute_show_databases(const ShowStmt & show_stmt);

    /**
     * @brief 执行 SHOW COLLECTIONS 语句
     * @param show_stmt 显示语句节点
     * @return 执行结果
     */
    ExecutorResult execute_show_collections(const ShowStmt & show_stmt);

    /**
     * @brief 执行 SHOW INDEXES 语句
     * @param show_stmt 显示语句节点
     * @return 执行结果
     */
    ExecutorResult execute_show_indexes(const ShowStmt & show_stmt);

    /**
     * @brief 执行 SHOW VINDEXES 语句
     * @param show_vindex_stmt 显示虚拟索引语句节点
     * @return 执行结果
     */
    ExecutorResult execute_show_vindexes(const ShowStmt & show_stmt);

    /** 辅助方法 - 统一访问入口，便于未来扩展 */

    /**
     * @brief 获取当前数据库
     * @return 当前数据库指针，如果未选择数据库返回 nullptr
     * @details 统一入口，未来可以在这里注入事务等上下文
     */
    Database * get_current_database();

    /**
     * @brief 获取集合
     * @param name 集合名称
     * @return 集合指针，如果数据库未选择或集合不存在返回 nullptr
     * @details 统一入口，未来可以在这里注入事务等上下文
     */
    Collection * get_collection(const std::string & name);

    /**
     * @brief 执行操作（模板方法，用于包装存储操作）
     * @param func 要执行的操作函数
     * @return 执行结果
     * @details 现在直接执行，未来可以在这里包装事务、日志等横切关注点
     * 
     * 使用示例：
     * ```cpp
     * return execute_with_context([&]() -> ExecutorResult {
     *     auto* collection = get_collection(collection_name);
     *     // 执行操作...
     *     return result;
     * });
     * ```
     */
    template<typename Func>
    ExecutorResult execute_with_context(Func && func);

private:
    std::unique_ptr<DatabaseManager> database_manager_;    // 数据库管理器引用
};

} // namespace dreamdb

#include "dreamdb/executor/executor.inl"
