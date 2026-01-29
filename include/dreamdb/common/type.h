#pragma once

#include <cstdint>
#include <variant>
#include <string>
#include <vector>

#include "dreamdb/common/null.h"

namespace dreamdb::common
{

/**
 * @brief 字段类型
 */
enum class FieldType : std::uint8_t
{
    TinyInt,            // 8 位整数
    SmallInt,           // 16 位整数
    Int,                // 32 位整数
    BigInt,             // 64 位整数
    Float,              // 浮点数
    Double,             // 双精度浮点数
    Char,               // 定长字符串
    VarChar,            // 变长字符串
    Boolean,            // 布尔值
    Timestamp,          // 时间戳
    Enum,               // 枚举
    Vector              // float 向量
};

/**
 * @brief 字段值类型
 */
using FieldValue = std::variant<
    std::int8_t,                    // TINYINT
    std::int16_t,                   // SMALLINT
    std::int32_t,                   // INTEGER
    std::int64_t,                   // BIGINT, TIMESTAMP
    float,                          // FLOAT
    double,                         // DOUBLE
    std::string,                    // CHAR, VARCHAR, ENUM
    bool,                           // BOOLEAN
    std::vector<float>,             // VECTOR
    Null                            // NULL
>;

/**
 * @brief 标量索引类型
 */
enum class IndexType : std::uint8_t
{
    BTREE,              // B-Tree 索引
    HASH                // Hash 索引
};

/**
 * @brief 向量索引类型
 */
enum class VIndexType : std::uint8_t
{
    FLAT,               // 线性扫描索引
    IVF_FLAT,           // 倒排平铺索引
    HNSW                // 分层导航小世界图索引
};

/**
 * @brief 向量距离度量类型
 */
enum class MetricType : std::uint8_t
{
    L2,                 // L2 距离
    IP,                 // 内积
    COSINE              // 余弦相似度
};

/**
 * @brief 排序方向
 */
enum class Direction : std::uint8_t
{
    ASC,                // 升序
    DESC                // 降序
};

/**
 * @brief 段状态
 */
enum class SegmentStatus : std::uint8_t
{
    GROWING,            // 正在增长，可以继续写入
    SEALED,             // 已密封，不再接受新的写入
    FLUSHED,            // 已刷新，数据已写入磁盘
    DROPPED             // 已删除
};

} // namespace dreamdb::common
