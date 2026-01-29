#pragma once

#include "dreamdb/executor/execution_result.h"

namespace dreamdb::binder::bound
{

class BoundStatement;

} // namespace dreamdb::binder::bound

namespace dreamdb::executor
{

/**
 * @brief 命令执行器
 * @details CommandExecutor 负责执行绑定后的语句，包括 SHOW、USE、CREATE、DROP、ALTER 等命令
 */
class CommandExecutor
{
public:
    explicit CommandExecutor();

    ~CommandExecutor() = default;

public:
    /**
     * @brief 执行绑定后的语句
     * @param bound_statement 绑定后的语句
     * @return 执行结果
     */
    ExecutionResult execute(binder::bound::BoundStatement & bound_statement);
};

} // namespace dreamdb::executor
