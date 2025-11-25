#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dreamdb
{

/**
 * @brief 字段类型
*/
enum class FieldType : std::uint8_t
{
    INT8,               // 8位整数
    INT16,              // 16位整数
    INT32,              // 32位整数
    INT64,              // 64位整数
    FLOAT,              // 浮点数
    DOUBLE,             // 双精度浮点数
    CHAR,               // 字符
    VARCHAR,            // 变长字符串
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

/**
 * @brief 空值类型
*/
class NullType
{
public:
    NullType() = default;

    ~NullType() = default;
};

using INT8 = std::int8_t;
using INT16 = std::int16_t;
using INT32 = std::int32_t;
using INT64 = std::int64_t;
using FLOAT = float;
using DOUBLE = double;
using CHAR = char;
using VARCHAR = std::string;
using BOOLEAN = bool;
using TIMESTAMP = std::int64_t;
using ENUM = std::string;
using FLOAT_VECTOR = std::vector<float>;
using NULL_TYPE = NullType;

/**
 * @brief 操作结果类
 */
class MutationResult
{
public:
    MutationResult() = default;

    ~MutationResult() = default;

public:
    bool success;                   // 操作是否成功
    std::string error_message;      // 错误信息
};

} // namespace dreamdb
