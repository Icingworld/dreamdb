#pragma once

#include <cstdint>
#include <vector>

#include "dreamdb/storage/row.h"

namespace dreamdb::executor
{

class ExecutionContext;

} // namespace dreamdb::executor

namespace dreamdb::planner::physical
{

/**
 * @brief 物理算子
 */
class PhysicalOperator
{
public:
    virtual ~PhysicalOperator() noexcept = default;

public:
    /**
     * @brief 打开算子
     * @param context 执行上下文
     */
    virtual void open(dreamdb::executor::ExecutionContext & context) = 0;

    /**
     * @brief 获取下一行
     * @param context 执行上下文
     * @param rowOut 行输出
     * @return 是否还有下一行
     */
    virtual bool next(dreamdb::executor::ExecutionContext & context, dreamdb::storage::Row & row) = 0;

    /**
     * @brief 关闭算子
     */
    virtual void close(dreamdb::executor::ExecutionContext & context) = 0;
};

} // namespace dreamdb::planner::physical
