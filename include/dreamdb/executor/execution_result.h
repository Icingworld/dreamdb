#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <variant>

#include "dreamdb/storage/row.h"

namespace dreamdb::executor
{

/**
 * @brief 查询结果
 * @details 用于 SELECT 语句的查询结果
 */
struct QueryResult
{
    std::vector<std::string> column_names;       // 列名
    std::vector<dreamdb::storage::Row> rows;     // 行数据
};

/**
 * @brief 修改结果
 * @details 用于 INSERT、UPDATE、DELETE 语句的修改结果
 */
struct MutationResult
{
    std::uint64_t affected_rows;      // 受影响的行数
};

/**
 * @brief 命令结果
 * @details 用于 CREATE、DROP、ALTER 等命令的执行结果
 */
struct CommandResult
{
    bool success;                      // 是否成功
    std::string message;               // 消息
};

/**
 * @brief 执行结果
 */
using ExecutionResult = std::variant<
    QueryResult, MutationResult, CommandResult
>;

} // namespace dreamdb::executor
