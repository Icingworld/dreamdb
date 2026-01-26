#pragma once

#include <vector>

#include "dreamdb/schema/field.h"

class ExecutionContext;
using Row = std::vector<dreamdb::FieldValue>;

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
    virtual void open(ExecutionContext & context) = 0;

    /**
     * @brief 获取下一行
     * @param context 执行上下文
     * @param rowOut 行输出
     * @return 是否还有下一行
     */
    virtual bool next(ExecutionContext & context, Row & row) = 0;

    /**
     * @brief 关闭算子
     */
    virtual void close(ExecutionContext & context) = 0;
};

} // namespace dreamdb::planner::physical
