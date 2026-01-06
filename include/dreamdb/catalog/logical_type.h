#pragma once

#include <cstdint>

namespace dreamdb
{

enum class LogicalTypeId : std::uint8_t
{
    LOGICAL_TYPE_INVALID,
    LOGICAL_TYPE_BOOLEAN,
    LOGICAL_TYPE_INTEGER,
    LOGICAL_TYPE_FLOAT,
    LOGICAL_TYPE_STRING,
    LOGICAL_TYPE_VECTOR,
    LOGICAL_TYPE_NULL
};

struct LogicalType
{
    LogicalTypeId id;        // 逻辑类型ID
    std::size_t width;       // 宽度，如向量维度
    bool nullable;           // 是否可为空
};

} // namespace dreamdb
