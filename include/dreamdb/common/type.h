#pragma once

#include <cstdint>

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
    IN_,                // 在列表中（避免与 Windows 宏 IN 冲突）
    LIKE                // 模糊匹配
};

/**
 * @brief 条件类型枚举
 */
enum class ConditionKind : std::uint8_t
{
    SINGLE,             // 单个值条件（EQ, NE, GT, GE, LT, LE）
    BETWEEN,            // BETWEEN 条件
    IN_,                // IN 条件（避免与 Windows 宏 IN 冲突）
    LIKE,               // LIKE 条件
    COMPOSITE,          // 组合条件（AND, OR）
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
 * @brief 段状态
 */
enum class SegmentStatus : std::uint8_t
{
    GROWING,            // 正在增长，可以继续写入
    SEALED,             // 已密封，不再接受新的写入
    FLUSHED,            // 已刷新，数据已写入磁盘
    DROPPED             // 已删除
};

} // namespace dreamdb
