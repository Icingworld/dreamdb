#pragma once

#include <cstdint>

namespace dreamdb
{

/**
 * @brief 字段类型
*/
enum class FieldType : std::uint8_t
{
    INT64,              // 64位整数
    FLOAT,              // 浮点数
    DOUBLE,             // 双精度浮点数
    CHAR,               // 字符
    VARCHAR,            // 变长字符串
    STRING,             // 字符串
    BOOLEAN,            // 布尔值
    TIMESTAMP,          // 时间戳
    ENUM,               // 枚举
    FLOAT_VECTOR        // 浮点向量
};

/**
 * @brief 向量距离度量类型
*/
enum class MetricType : std::uint8_t
{
    L2,                 // L2距离（欧氏距离）
    IP,                 // 内积（Inner Product）
    COSINE,             // 余弦相似度
    NONE                // 无距离度量
};

} // namespace dreamdb
