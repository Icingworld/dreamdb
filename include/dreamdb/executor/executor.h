#pragma once

#include <memory>

#include "dreamdb/common/mutation_result.h"
#include "dreamdb/planner/physical_planner/physical_plan_node.h"
#include "dreamdb/planner/physical_planner/select/physical_select_plan_node.h"
#include "dreamdb/planner/physical_planner/update/physical_update_plan_node.h"
#include "dreamdb/planner/physical_planner/delete/physical_delete_plan_node.h"
#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/binder/bound/bound_use_statement.h"
#include "dreamdb/binder/bound/bound_create_statement.h"
#include "dreamdb/binder/bound/bound_drop_statement.h"
#include "dreamdb/binder/bound/bound_alter_statement.h"
#include "dreamdb/binder/bound/bound_describe_statement.h"
#include "dreamdb/binder/bound/bound_show_statement.h"
#include "dreamdb/binder/bound/bound_insert_statement.h"

namespace dreamdb
{

class DatabaseManager;

/**
 * @brief 执行器
 * @details Executor 负责执行 SQL 语句
 * - DML/DQL 语句经过 Planner 优化后，以 PhysicalPlanNode 形式执行
 * - DDL 语句直接以 BoundStatement 形式执行
 */
class Executor
{
public:
    /**
     * @brief 构造函数
     * @param database_manager 数据库管理器（所有权将被转移）
     */
    explicit Executor(std::unique_ptr<DatabaseManager> database_manager);

    Executor(const Executor &) = delete;

    Executor(Executor &&) noexcept = default;

    Executor & operator=(const Executor &) = delete;

    Executor & operator=(Executor &&) noexcept = default;

    ~Executor() = default;

public:
    /**
     * @brief 执行物理计划（用于 DML/DQL 语句，经过 planner 优化后）
     * @param physical_plan 物理计划节点
     * @return 执行结果
     */
    MutationResult execute(const PhysicalPlanNode & physical_plan);

    /**
     * @brief 执行绑定语句（用于 DDL 语句，直接执行）
     * @param bound_statement 绑定后的语句
     * @return 执行结果
     */
    MutationResult execute(const BoundStatement & bound_statement);

private:
    /** Physical Planner 对应执行方法 */

    /**
     * @brief 执行 SELECT 物理计划
     * @param select_plan SELECT 物理计划节点
     * @return 执行结果
     */
    MutationResult execute_select(const PhysicalSelectPlanNode & select_plan);

    /**
     * @brief 执行 UPDATE 物理计划
     * @param update_plan UPDATE 物理计划节点
     * @return 执行结果
     */
    MutationResult execute_update(const PhysicalUpdatePlanNode & update_plan);

    /**
     * @brief 执行 DELETE 物理计划
     * @param delete_plan DELETE 物理计划节点
     * @return 执行结果
     */
    MutationResult execute_delete(const PhysicalDeletePlanNode & delete_plan);

    /** Bound Statement 对应执行方法 */

    /**
     * @brief 执行 USE 语句
     * @param use_statement 绑定后的 USE 语句
     * @return 执行结果
     */
    MutationResult execute_use(const BoundUseStatement & use_statement);

    /**
     * @brief 执行 CREATE 语句
     * @param create_statement 绑定后的 CREATE 语句
     * @return 执行结果
     */
    MutationResult execute_create(const BoundCreateStatement & create_statement);

    /**
     * @brief 执行 DROP 语句
     * @param drop_statement 绑定后的 DROP 语句
     * @return 执行结果
     */
    MutationResult execute_drop(const BoundDropStatement & drop_statement);

    /**
     * @brief 执行 DROP DATABASE 语句
     * @param database_id 数据库 ID
     * @param if_exists 如果不存在是否忽略错误
     * @return 执行结果
     */
    MutationResult execute_drop_database(std::size_t database_id, bool if_exists);

    /**
     * @brief 执行 DROP COLLECTION 语句
     * @param collection_id 集合 ID
     * @param if_exists 如果不存在是否忽略错误
     * @return 执行结果
     */
    MutationResult execute_drop_collection(std::size_t collection_id, bool if_exists);

    /**
     * @brief 执行 DROP INDEX 语句
     * @param collection_id 集合 ID
     * @param index_name 索引名称
     * @param if_exists 如果不存在是否忽略错误
     * @return 执行结果
     */
    MutationResult execute_drop_index(std::size_t collection_id, const std::string & index_name, bool if_exists);

    /**
     * @brief 执行 DROP VINDEX 语句
     * @param collection_id 集合 ID
     * @param vindex_name 向量索引名称
     * @param if_exists 如果不存在是否忽略错误
     * @return 执行结果
     */
    MutationResult execute_drop_vindex(std::size_t collection_id, const std::string & vindex_name, bool if_exists);

    /**
     * @brief 执行 ALTER 语句
     * @param alter_statement 绑定后的 ALTER 语句
     * @return 执行结果
     */
    MutationResult execute_alter(const BoundAlterStatement & alter_statement);

    /**
     * @brief 执行 DESCRIBE 语句
     * @param describe_statement 绑定后的 DESCRIBE 语句
     * @return 执行结果
     */
    MutationResult execute_describe(const BoundDescribeStatement & describe_statement);

    /**
     * @brief 执行 SHOW 语句
     * @param show_statement 绑定后的 SHOW 语句
     * @return 执行结果
     */
    MutationResult execute_show(const BoundShowStatement & show_statement);

    /**
     * @brief 执行 SHOW DATABASES 语句
     * @return 执行结果
     */
    MutationResult execute_show_databases();

    /**
     * @brief 执行 SHOW COLLECTIONS 语句
     * @param database_id 数据库 ID（可选）
     * @return 执行结果
     */
    MutationResult execute_show_collections(std::optional<std::size_t> database_id);

    /**
     * @brief 执行 SHOW INDEXES 语句
     * @param collection_id 集合 ID
     * @param database_id 数据库 ID（可选）
     * @return 执行结果
     */
    MutationResult execute_show_indexes(std::size_t collection_id, std::optional<std::size_t> database_id);

    /**
     * @brief 执行 SHOW VINDEXES 语句
     * @param collection_id 集合 ID
     * @param database_id 数据库 ID（可选）
     * @return 执行结果
     */
    MutationResult execute_show_vindexes(std::size_t collection_id, std::optional<std::size_t> database_id);

    /**
     * @brief 执行 INSERT 语句
     * @param insert_statement 绑定后的 INSERT 语句
     * @return 执行结果
     */
    MutationResult execute_insert(const BoundInsertStatement & insert_statement);

private:
    std::unique_ptr<DatabaseManager> database_manager_;  // 数据库管理器
};

} // namespace dreamdb
