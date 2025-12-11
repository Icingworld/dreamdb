#pragma once

#include <cstdint>
#include <string>

namespace dreamdb
{

/**
 * @brief 字段类型
 */
enum class FieldType : std::uint8_t
{
    TINYINT,            // 8 位整数
    SMALLINT,           // 16 位整数
    INTEGER,            // 32 位整数
    BIGINT,             // 64 位整数
    FLOAT,              // 浮点数
    DOUBLE,             // 双精度浮点数
    DECIMAL,            // 高精度浮点数
    CHAR,               // 定长字符串
    VARCHAR,            // 变长字符串
    BOOLEAN,            // 布尔值
    TIMESTAMP,          // 时间戳
    ENUM,               // 枚举
    VECTOR              // float 向量
};

/**
 * @brief 标量索引类型
 */
enum class IndexType : std::uint8_t
{
    BTREE,              // B-Tree 索引
    HASH,               // Hash 索引
    NONE                // 无索引
};

/**
 * @brief 向量索引类型
 */
enum class VIndexType : std::uint8_t
{
    FLAT,               // 线性扫描索引
    IVF_FLAT,           // 倒排平铺索引
    HNSW,               // 分层导航小世界图索引
    NONE                // 无索引
};

/**
 * @brief 向量距离度量类型
 */
enum class MetricType : std::uint8_t
{
    L2,                 // L2 距离
    IP,                 // 内积
    COSINE,             // 余弦相似度
    NONE                // 无距离度量
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
 * @brief 条件类型
 */
enum class ConditionType : std::uint8_t
{
    EQ,                 // 等于
    NE,                 // 不等于
    GT,                 // 大于
    GE,                 // 大于等于
    LT,                 // 小于
    LE,                 // 小于等于
    BETWEEN,            // 在范围内
    IN,                 // 在列表中
    LIKE                // 模糊匹配
};

/**
 * @brief 条件类型枚举
 */
enum class ConditionKind : std::uint8_t
{
    SINGLE,         // 单个值条件（EQ, NE, GT, GE, LT, LE）
    BETWEEN,        // BETWEEN 条件
    IN,             // IN 条件
    LIKE,           // LIKE 条件
    COMPOSITE       // 组合条件（AND, OR）
};

/**
 * @brief 逻辑操作符
 */
enum class LogicOperator : std::uint8_t
{
    AND,                // 与
    OR                  // 或
};

/**
 * @brief 操作结果类
 */
class MutationResult
{
public:
    MutationResult() = default;

    ~MutationResult() = default;

public:
    bool success = false;                   // 操作是否成功
    std::string error_message;              // 错误信息
    std::size_t affected_count = 0;         // 受影响的记录数
};

} // namespace dreamdb
