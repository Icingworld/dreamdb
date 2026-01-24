#pragma once

#include <string>
#include <vector>
#include <optional>

#include "dreamdb/planner/logical/logical_operator.h"

namespace dreamdb::binder::bound
{

class BoundExpression;

} // namespace dreamdb::binder::bound

namespace dreamdb::planner::logical
{

/**
 * @brief 逻辑投影项
 * @details 用于表示投影的项
 */
struct LogicalProjectItem
{
    std::unique_ptr<dreamdb::binder::bound::BoundExpression> expression;   // 投影表达式
    std::optional<std::string> alias;          // 别名
};

/**
 * @brief 逻辑投影算子
 * @details 用于投影的算子
 */
class LogicalProject final : public LogicalOperator
{
public:
    LogicalProject(std::vector<LogicalProjectItem> project_items);

    ~LogicalProject() noexcept override = default;

public:
    /**
     * @brief 获取投影项数量
     * @return 投影项数量
     */
    std::size_t project_item_count() const noexcept;

    /**
     * @brief 获取投影项
     * @param index 投影项索引
     * @return 投影项
     */
    const LogicalProjectItem & project_item_at(std::size_t index) const noexcept;

private:
    std::vector<LogicalProjectItem> project_items_;    // 投影项列表
};

} // namespace dreamdb::planner::logical
