#pragma once

#include <string>
#include <vector>
#include <cstddef>

#include "dreamdb/parser/ast/ast_node.h"
#include "dreamdb/schema/entity.h"

namespace dreamdb
{

// 向前声明
class SelectStmt;
class DeleteStmt;
class InsertStmt;
class UpdateStmt;
class CreateStmt;
class DropStmt;

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
    void set_success(bool success) noexcept;

    /**
     * @brief 检查执行是否成功
     * @return 如果成功返回 true
     */
    bool is_success() const noexcept;

    /**
     * @brief 设置错误信息
     * @param message 错误消息
     */
    void set_error_message(const std::string & message);

    /**
     * @brief 获取错误信息
     * @return 错误消息
     */
    const std::string & get_error_message() const noexcept;

    /**
     * @brief 设置受影响的行数（用于 INSERT/UPDATE/DELETE）
     * @param count 受影响的行数
     */
    void set_affected_count(std::size_t count) noexcept;

    /**
     * @brief 获取受影响的行数
     * @return 受影响的行数
     */
    std::size_t get_affected_count() const noexcept;

    /**
     * @brief 添加查询结果行（用于 SELECT）
     * @param entity 实体
     */
    void add_row(Entity && entity);

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

    /**
     * @brief 清空所有结果
     */
    void clear() noexcept;

private:
    bool success;                           // 执行是否成功
    std::string error_message;              // 错误信息
    std::size_t affected_count;             // 受影响的行数
    std::vector<Entity> rows;               // 查询结果
};

/**
 * @brief 执行器
 */
class Executor
{
public:
    Executor() = default;

    ~Executor() = default;

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

private:
};

} // namespace dreamdb
