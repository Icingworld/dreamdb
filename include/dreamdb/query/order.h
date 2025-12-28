#pragma once

#include <cstdint>

#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 排序类
 */
class Order
{
public:
    Order() = delete;

    Order(std::uint8_t field_index, Direction direction) noexcept;

    Order(const Order & other) = default;

    Order(Order && other) noexcept = default;

    Order & operator=(const Order & other) = default;

    Order & operator=(Order && other) noexcept = default;

    ~Order() = default;

public:
    /**
     * @brief 获取字段索引
     * @return 字段索引
     */
    std::uint8_t get_field_index() const noexcept;

    /**
     * @brief 获取排序方向
     * @return 排序方向
     */
    Direction get_direction() const noexcept;

private:
    std::uint8_t field_index_;      // 字段索引
    Direction direction_;           // 排序方向
};

} // namespace dreamdb
