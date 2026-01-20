#pragma once

#include <cstdint>
#include <memory>

#include "dreamdb/binder/bound/expression/expression.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的一元运算符类型
 */
enum class BoundUnaryOperatorType : std::uint8_t
{
    Not,        // 逻辑非
    Minus,      // 算数负号
    Plus        // 算数正号
};

/**
 * @brief 绑定后的一元表达式
 */
class BoundUnaryExpression final : public BoundExpression
{
public:
    explicit BoundUnaryExpression(
        BoundUnaryOperatorType operator_type,
        std::unique_ptr<BoundExpression> operand,
        dreamdb::common::LogicalType logical_type
    );

    ~BoundUnaryExpression() noexcept override = default;

public:
    /**
     * @brief 获取一元运算符类型
     * @return 一元运算符类型
     */
    BoundUnaryOperatorType operator_type() const noexcept;

    /**
     * @brief 获取操作数
     * @return 操作数表达式
     */
    const BoundExpression & operand() const noexcept;

private:
    BoundUnaryOperatorType operator_type_;         // 一元运算符类型
    std::unique_ptr<BoundExpression> operand_;     // 操作数表达式
};

} // namespace dreamdb::binder::bound
