#pragma once

#include <cstdint>
#include <string>

namespace dreamdb
{

/**
 * @brief 时间戳类型
 * @details 用于表示 TIMESTAMP 类型，内部使用 Unix 时间戳（毫秒）存储
 *          支持从字符串解析和转换为字符串（ISO 8601 格式）
 */
class Timestamp
{
public:
    /**
     * @brief 默认构造函数：创建表示当前时间的 Timestamp
     */
    Timestamp();

    /**
     * @brief 构造函数：从 Unix 时间戳（毫秒）构造
     * @param value Unix 时间戳（毫秒，自 1970-01-01 00:00:00 UTC）
     */
    explicit Timestamp(std::int64_t value);

    /**
     * @brief 构造函数：从字符串构造
     * @param str 字符串表示，支持以下格式：
     *            - ISO 8601: "2025-01-01T12:00:00" 或 "2025-01-01T12:00:00.123"
     *            - Unix 时间戳（毫秒）: "1736217600000"
     * @throw std::invalid_argument 如果字符串格式无效
     */
    explicit Timestamp(const std::string & str);

    Timestamp(const Timestamp & other) = default;

    Timestamp(Timestamp && other) noexcept = default;

    Timestamp & operator=(const Timestamp & other) = default;

    Timestamp & operator=(Timestamp && other) noexcept = default;

    ~Timestamp() = default;

public:
    /**
     * @brief 获取 Unix 时间戳（毫秒）
     * @return Unix 时间戳（毫秒）
     */
    std::int64_t value() const noexcept;

    /**
     * @brief 转换为字符串
     * @return ISO 8601 格式的字符串（如 "2024-01-01T12:00:00.123"）
     */
    std::string to_string() const;

    /**
     * @brief 从字符串解析 Timestamp
     * @param str 字符串表示
     * @return Timestamp 对象
     * @throw std::invalid_argument 如果字符串格式无效
     */
    static Timestamp from_string(const std::string & str);

    /**
     * @brief 获取当前时间的 Timestamp
     * @return 当前时间的 Timestamp
     */
    static Timestamp now();

public:
    bool operator==(const Timestamp & other) const noexcept;

    bool operator!=(const Timestamp & other) const noexcept;

    bool operator<(const Timestamp & other) const noexcept;

    bool operator<=(const Timestamp & other) const noexcept;

    bool operator>(const Timestamp & other) const noexcept;

    bool operator>=(const Timestamp & other) const noexcept;

private:
    /**
     * @brief 从 ISO 8601 格式字符串解析
     * @param str ISO 8601 格式字符串
     * @return Unix 时间戳（毫秒）
     * @throw std::invalid_argument 如果格式无效
     */
    static std::int64_t parse_iso8601(const std::string & str);

    /**
     * @brief 从 Unix 时间戳字符串解析
     * @param str Unix 时间戳字符串（毫秒）
     * @return Unix 时间戳（毫秒）
     * @throw std::invalid_argument 如果格式无效
     */
    static std::int64_t parse_timestamp(const std::string & str);

private:
    std::int64_t value_;  // Unix 时间戳（毫秒，自 1970-01-01 00:00:00 UTC）
};

} // namespace dreamdb
