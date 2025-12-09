#include "dreamdb/common/timestamp.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#include <time.h>
#define timegm _mkgmtime
#else
#include <time.h>
extern "C" {
    time_t timegm(struct tm * tm);
}
#endif

namespace dreamdb
{

Timestamp::Timestamp()
{
    // 获取当前时间的 Unix 时间戳（毫秒）
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    value_ = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

Timestamp::Timestamp(std::int64_t value)
    : value_(value)
{
}

Timestamp::Timestamp(const std::string & str)
{
    value_ = from_string(str).value_;
}

std::int64_t Timestamp::value() const noexcept
{
    return value_;
}

std::string Timestamp::to_string() const
{
    // 将毫秒时间戳转换为 time_t（秒）
    std::time_t time_seconds = value_ / 1000;
    int milliseconds = static_cast<int>(value_ % 1000);

    // 转换为 UTC 时间
    std::tm tm_buf;
#ifdef _WIN32
    errno_t err = gmtime_s(&tm_buf, &time_seconds);
    if (err != 0) {
        throw std::runtime_error("Failed to convert timestamp to UTC time");
    }
#else
    if (gmtime_r(&time_seconds, &tm_buf) == nullptr) {
        throw std::runtime_error("Failed to convert timestamp to UTC time");
    }
#endif

    // 格式化为 ISO 8601 格式: YYYY-MM-DDTHH:MM:SS.mmm
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << (tm_buf.tm_year + 1900) << '-'
        << std::setw(2) << (tm_buf.tm_mon + 1) << '-'
        << std::setw(2) << tm_buf.tm_mday << 'T'
        << std::setw(2) << tm_buf.tm_hour << ':'
        << std::setw(2) << tm_buf.tm_min << ':'
        << std::setw(2) << tm_buf.tm_sec << '.'
        << std::setw(3) << milliseconds;

    return oss.str();
}

Timestamp Timestamp::from_string(const std::string & str)
{
    if (str.empty()) {
        throw std::invalid_argument("Empty timestamp string");
    }

    // 尝试解析为 Unix 时间戳（纯数字）
    bool is_numeric = true;
    for (char c : str) {
        if (c < '0' || c > '9') {
            is_numeric = false;
            break;
        }
    }

    if (is_numeric) {
        return Timestamp(parse_timestamp(str));
    } else {
        // 尝试解析为 ISO 8601 格式
        return Timestamp(parse_iso8601(str));
    }
}

Timestamp Timestamp::now()
{
    return Timestamp();
}

bool Timestamp::operator==(const Timestamp & other) const noexcept
{
    return value_ == other.value_;
}

bool Timestamp::operator!=(const Timestamp & other) const noexcept
{
    return value_ != other.value_;
}

bool Timestamp::operator<(const Timestamp & other) const noexcept
{
    return value_ < other.value_;
}

bool Timestamp::operator<=(const Timestamp & other) const noexcept
{
    return value_ <= other.value_;
}

bool Timestamp::operator>(const Timestamp & other) const noexcept
{
    return value_ > other.value_;
}

bool Timestamp::operator>=(const Timestamp & other) const noexcept
{
    return value_ >= other.value_;
}

std::int64_t Timestamp::parse_iso8601(const std::string & str)
{
    // 支持的格式：
    // - "2024-01-01T12:00:00"
    // - "2024-01-01T12:00:00.123"
    // - "2024-01-01T12:00:00.123Z"

    std::tm tm = {};
    int milliseconds = 0;
    std::istringstream iss(str);
    char delimiter;

    // 解析日期部分: YYYY-MM-DD
    if (!(iss >> tm.tm_year >> delimiter) || delimiter != '-' ||
        !(iss >> tm.tm_mon >> delimiter) || delimiter != '-' ||
        !(iss >> tm.tm_mday)) {
        throw std::invalid_argument("Invalid ISO 8601 date format: " + str);
    }
    tm.tm_year -= 1900;  // tm_year 是从 1900 开始的年份
    tm.tm_mon -= 1;      // tm_mon 是 0-11

    // 解析 'T' 分隔符
    if (!(iss >> delimiter) || delimiter != 'T') {
        throw std::invalid_argument("Invalid ISO 8601 timestamp format: missing 'T': " + str);
    }

    // 解析时间部分: HH:MM:SS
    if (!(iss >> tm.tm_hour >> delimiter) || delimiter != ':' ||
        !(iss >> tm.tm_min >> delimiter) || delimiter != ':' ||
        !(iss >> tm.tm_sec)) {
        throw std::invalid_argument("Invalid ISO 8601 time format: " + str);
    }

    // 解析毫秒部分（如果存在）
    if (iss.peek() == '.') {
        iss.ignore();  // 跳过 '.'
        std::string ms_str;
        int ms_digits = 0;
        while (iss.peek() >= '0' && iss.peek() <= '9' && ms_digits < 3) {
            ms_str += static_cast<char>(iss.get());
            ++ms_digits;
        }
        if (!ms_str.empty()) {
            milliseconds = std::stoi(ms_str);
            // 补齐到3位数字
            while (ms_digits < 3) {
                milliseconds *= 10;
                ++ms_digits;
            }
        }
        // 跳过多余的毫秒数字
        while (iss.peek() >= '0' && iss.peek() <= '9') {
            iss.ignore();
        }
    }

    // 跳过 'Z' 或时区信息（简化处理，假设都是 UTC）
    if (iss.peek() == 'Z') {
        iss.ignore();
    }

    // 转换为 Unix 时间戳（秒）
    std::tm tm_utc = tm;
    tm_utc.tm_isdst = 0;  // 不使用夏令时
    std::time_t time_seconds = timegm(&tm_utc);  // Windows 上已定义为 _mkgmtime

    if (time_seconds == -1) {
        throw std::invalid_argument("Invalid timestamp value: " + str);
    }

    // 转换为毫秒并加上毫秒部分
    return static_cast<std::int64_t>(time_seconds) * 1000 + milliseconds;
}

std::int64_t Timestamp::parse_timestamp(const std::string & str)
{
    try {
        std::size_t pos = 0;
        std::int64_t value = std::stoll(str, &pos);

        // 检查是否完全解析
        if (pos != str.length()) {
            throw std::invalid_argument("Invalid timestamp string: " + str);
        }

        return value;
    } catch (const std::exception & e) {
        throw std::invalid_argument("Invalid timestamp string: " + str);
    }
}

} // namespace dreamdb

