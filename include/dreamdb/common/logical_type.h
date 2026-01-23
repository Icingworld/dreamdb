#pragma once

#include <cstdint>

namespace dreamdb::common
{

/**
 * @brief 逻辑类型ID
 */
enum class LogicalTypeId : std::uint8_t
{
    Invalid,                  // 无效类型
    Boolean,                  // 布尔类型
    Integer,                  // 整数类型
    Float,                    // 浮点数类型
    String,                   // 字符串类型
    Vector,                   // 向量类型
    Null                      // 空类型
};

/**
 * @brief 逻辑类型
 */
struct LogicalType
{
    LogicalTypeId id;        // 逻辑类型ID
};

} // namespace dreamdb::common
