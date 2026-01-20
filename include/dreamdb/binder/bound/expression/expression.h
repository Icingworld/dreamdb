#pragma once

#include <cstdint>

#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的表达式类型
 */
enum class BoundExpressionType : std::uint8_t
{
    ColumnReference,              // 字段引用
    Constant,                     // 常量
    Binary,                       // 二元运算符
    Unary,                        // 一元运算符
    FunctionCall,                 // 函数
    In,                           // IN 表达式
    Like,                         // LIKE 表达式
    Between,                      // BETWEEN 表达式
    Cast                          // 类型转换
};

/**
 * @brief 绑定后的表达式
 */
class BoundExpression
{
protected:
    explicit BoundExpression(BoundExpressionType expression_type, dreamdb::common::LogicalType logical_type);

public:
    virtual ~BoundExpression() noexcept = default;

public:
    /**
     * @brief 获取表达式类型
     * @return 表达式类型
     */
    BoundExpressionType expression_type() const noexcept;

    /**
     * @brief 获取逻辑类型
     * @return 逻辑类型
     */
    dreamdb::common::LogicalType logical_type() const noexcept;

private:
    BoundExpressionType expression_type_;              // 表达式类型
    dreamdb::common::LogicalType logical_type_;        // 逻辑类型
};

} // namespace dreamdb::binder::bound
