#include "dreamdb/common/decimal.h"

#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cctype>

namespace dreamdb
{

namespace
{

/**
 * @brief 计算 10 的 n 次方
 */
std::int64_t power_of_10(int n)
{
    if (n < 0 || n > 18) {
        throw std::invalid_argument("Scale factor out of range");
    }
    
    std::int64_t result = 1;
    for (int i = 0; i < n; ++i) {
        result *= 10;
    }
    return result;
}

/**
 * @brief 检查乘法是否溢出
 */
bool multiply_would_overflow(std::int64_t a, std::int64_t b)
{
    if (a == 0 || b == 0) {
        return false;
    }
    
    // 检查 a * b 是否超过 int64_t 最大值
    if (a > std::numeric_limits<std::int64_t>::max() / b) {
        return true;
    }
    if (a < std::numeric_limits<std::int64_t>::min() / b) {
        return true;
    }
    
    return false;
}

} // namespace

Decimal::Decimal(std::int64_t value, int precision, int scale)
    : value_(value)
    , precision_(precision)
    , scale_(scale)
{
    validate_precision_scale(precision, scale);
    
    // 检查值是否在精度范围内
    if (!is_in_range(value, precision, scale)) {
        throw std::overflow_error("Decimal value out of precision range");
    }
}

Decimal::Decimal(double value, int precision, int scale)
    : precision_(precision)
    , scale_(scale)
{
    validate_precision_scale(precision, scale);
    
    // 计算缩放因子
    std::int64_t scale_factor = compute_scale_factor(scale);
    
    // 将 double 转换为缩放后的整数
    // 使用 round 确保正确的四舍五入
    double scaled = value * static_cast<double>(scale_factor);
    
    // 检查是否超出 int64_t 范围
    if (scaled > static_cast<double>(std::numeric_limits<std::int64_t>::max())) {
        throw std::overflow_error("Decimal value exceeds maximum");
    }
    if (scaled < static_cast<double>(std::numeric_limits<std::int64_t>::min())) {
        throw std::overflow_error("Decimal value below minimum");
    }
    
    value_ = static_cast<std::int64_t>(std::round(scaled));
    
    // 检查值是否在精度范围内
    if (!is_in_range(value_, precision, scale)) {
        throw std::overflow_error("Decimal value out of precision range");
    }
}

Decimal::Decimal(const std::string & str, int precision, int scale)
    : precision_(precision)
    , scale_(scale)
{
    validate_precision_scale(precision, scale);
    value_ = parse_string(str, scale);
    
    // 检查值是否在精度范围内
    if (!is_in_range(value_, precision, scale)) {
        throw std::overflow_error("Decimal value out of precision range");
    }
}

std::int64_t Decimal::value() const noexcept
{
    return value_;
}

int Decimal::precision() const noexcept
{
    return precision_;
}

int Decimal::scale() const noexcept
{
    return scale_;
}

double Decimal::to_double() const noexcept
{
    std::int64_t scale_factor = compute_scale_factor(scale_);
    return static_cast<double>(value_) / static_cast<double>(scale_factor);
}

std::string Decimal::to_string() const
{
    if (value_ == 0) {
        if (scale_ == 0) {
            return "0";
        }
        // 对于 scale > 0 的情况，返回 "0.00..." 格式
        std::string result = "0.";
        result.append(scale_, '0');
        return result;
    }
    
    std::int64_t scale_factor = compute_scale_factor(scale_);
    std::int64_t abs_value = std::abs(value_);
    
    std::int64_t integer_part = abs_value / scale_factor;
    std::int64_t fractional_part = abs_value % scale_factor;
    
    std::ostringstream oss;
    
    // 处理负数
    if (value_ < 0) {
        oss << "-";
    }
    
    // 整数部分
    oss << integer_part;
    
    // 小数部分
    if (scale_ > 0) {
        oss << ".";
        
        // 将小数部分格式化为固定宽度（补零）
        std::ostringstream frac_oss;
        frac_oss << std::setfill('0') << std::setw(scale_) << fractional_part;
        std::string frac_str = frac_oss.str();
        
        // 移除尾部的零（但至少保留一位，如果全部为零则保留所有零）
        std::size_t last_non_zero = frac_str.find_last_not_of('0');
        if (last_non_zero != std::string::npos) {
            frac_str = frac_str.substr(0, last_non_zero + 1);
        }
        
        oss << frac_str;
    }
    
    return oss.str();
}

Decimal Decimal::operator+(const Decimal & other) const
{
    // 对齐 scale
    int aligned_scale = align_scale(*this, other);
    
    std::int64_t this_value = scale_value(value_, scale_, aligned_scale);
    std::int64_t other_value = scale_value(other.value_, other.scale_, aligned_scale);
    
    // 检查加法是否溢出
    if ((this_value > 0 && other_value > std::numeric_limits<std::int64_t>::max() - this_value) ||
        (this_value < 0 && other_value < std::numeric_limits<std::int64_t>::min() - this_value)) {
        throw std::overflow_error("Decimal addition overflow");
    }
    
    std::int64_t result_value = this_value + other_value;
    int result_precision = std::max(precision_, other.precision_);
    
    if (!is_in_range(result_value, result_precision, aligned_scale)) {
        throw std::overflow_error("Decimal result out of precision range");
    }
    
    return Decimal(result_value, result_precision, aligned_scale);
}

Decimal Decimal::operator-(const Decimal & other) const
{
    // 对齐 scale
    int aligned_scale = align_scale(*this, other);
    
    std::int64_t this_value = scale_value(value_, scale_, aligned_scale);
    std::int64_t other_value = scale_value(other.value_, other.scale_, aligned_scale);
    
    // 检查减法是否溢出
    if ((this_value > 0 && other_value < std::numeric_limits<std::int64_t>::min() + this_value) ||
        (this_value < 0 && other_value > std::numeric_limits<std::int64_t>::max() + this_value)) {
        throw std::overflow_error("Decimal subtraction overflow");
    }
    
    std::int64_t result_value = this_value - other_value;
    int result_precision = std::max(precision_, other.precision_);
    
    if (!is_in_range(result_value, result_precision, aligned_scale)) {
        throw std::overflow_error("Decimal result out of precision range");
    }
    
    return Decimal(result_value, result_precision, aligned_scale);
}

Decimal Decimal::operator*(const Decimal & other) const
{
    // 检查乘法是否溢出
    if (multiply_would_overflow(value_, other.value_)) {
        throw std::overflow_error("Decimal multiplication overflow");
    }
    
    std::int64_t result_value = value_ * other.value_;
    int result_precision = precision_ + other.precision_;
    int result_scale = scale_ + other.scale_;
    
    // 检查结果是否在范围内
    if (!is_in_range(result_value, result_precision, result_scale)) {
        throw std::overflow_error("Decimal result out of precision range");
    }
    
    return Decimal(result_value, result_precision, result_scale);
}

Decimal Decimal::operator/(const Decimal & other) const
{
    if (other.value_ == 0) {
        throw std::domain_error("Division by zero");
    }
    
    // 为了保持精度，我们需要将被除数放大
    // 使用被除数的 scale 作为结果的 scale
    std::int64_t scale_factor = compute_scale_factor(scale_);
    
    // 检查乘法是否溢出
    if (multiply_would_overflow(value_, scale_factor)) {
        throw std::overflow_error("Decimal division overflow");
    }
    
    // 执行除法：先将被除数乘以 scale_factor，然后除以除数
    // 这样可以保持被除数的 scale
    std::int64_t scaled_dividend = value_ * scale_factor;
    
    // 执行整数除法（会向下取整）
    std::int64_t result_value = scaled_dividend / other.value_;
    
    // 处理负数除法的舍入（向零舍入）
    if ((scaled_dividend < 0) != (other.value_ < 0)) {
        // 如果结果应该是负数，但整数除法向下取整了，需要调整
        // 这里我们保持整数除法的行为（向零舍入）
    }
    
    // 结果使用被除数的 precision 和 scale
    if (!is_in_range(result_value, precision_, scale_)) {
        throw std::overflow_error("Decimal result out of precision range");
    }
    
    return Decimal(result_value, precision_, scale_);
}

Decimal Decimal::operator%(const Decimal & other) const
{
    if (other.value_ == 0) {
        throw std::domain_error("Modulo by zero");
    }
    
    // 对齐 scale
    int aligned_scale = align_scale(*this, other);
    
    std::int64_t this_value = scale_value(value_, scale_, aligned_scale);
    std::int64_t other_value = scale_value(other.value_, other.scale_, aligned_scale);
    
    std::int64_t result_value = this_value % other_value;
    int result_precision = std::max(precision_, other.precision_);
    
    return Decimal(result_value, result_precision, aligned_scale);
}

Decimal Decimal::operator-() const
{
    // 检查取负是否溢出（只有最小值取负会溢出）
    if (value_ == std::numeric_limits<std::int64_t>::min()) {
        throw std::overflow_error("Decimal negation overflow");
    }
    
    return Decimal(-value_, precision_, scale_);
}

bool Decimal::operator==(const Decimal & other) const
{
    // 对齐 scale 后比较
    int aligned_scale = align_scale(*this, other);
    
    std::int64_t this_value = scale_value(value_, scale_, aligned_scale);
    std::int64_t other_value = scale_value(other.value_, other.scale_, aligned_scale);
    
    return this_value == other_value;
}

bool Decimal::operator!=(const Decimal & other) const
{
    return !(*this == other);
}

bool Decimal::operator<(const Decimal & other) const
{
    // 对齐 scale 后比较
    int aligned_scale = align_scale(*this, other);
    
    std::int64_t this_value = scale_value(value_, scale_, aligned_scale);
    std::int64_t other_value = scale_value(other.value_, other.scale_, aligned_scale);
    
    return this_value < other_value;
}

bool Decimal::operator<=(const Decimal & other) const
{
    return *this < other || *this == other;
}

bool Decimal::operator>(const Decimal & other) const
{
    return !(*this <= other);
}

bool Decimal::operator>=(const Decimal & other) const
{
    return !(*this < other);
}

void Decimal::validate_precision_scale(int precision, int scale)
{
    if (precision <= 0) {
        throw std::invalid_argument("Precision must be positive");
    }
    
    if (scale < 0) {
        throw std::invalid_argument("Scale must be non-negative");
    }
    
    if (scale > precision) {
        throw std::invalid_argument("Scale cannot exceed precision");
    }
    
    // 限制最大精度和 scale（避免溢出）
    if (precision > 38) {
        throw std::invalid_argument("Precision cannot exceed 38");
    }
    
    if (scale > 18) {
        throw std::invalid_argument("Scale cannot exceed 18 (to avoid int64_t overflow)");
    }
}

std::int64_t Decimal::compute_scale_factor(int scale)
{
    return power_of_10(scale);
}

bool Decimal::is_in_range(std::int64_t value, int precision, int scale)
{
    // 计算整数部分的最大位数
    int integer_digits = precision - scale;
    
    // 如果 integer_digits <= 0，说明只有小数部分
    if (integer_digits <= 0) {
        // 如果 precision == scale，则整数部分只能是 0
        // 最大值应该是 scale_factor - 1（例如 scale=2，最大值为 99）
        if (scale == 0) {
            return value == 0;
        }
        std::int64_t scale_factor = compute_scale_factor(scale);
        return std::abs(value) < scale_factor;
    }
    
    // 计算整数部分的最大值（10^integer_digits - 1）
    std::int64_t max_integer = power_of_10(integer_digits) - 1;
    std::int64_t scale_factor = compute_scale_factor(scale);
    
    // 计算最大值：max_integer * scale_factor + (scale_factor - 1)
    // 例如：DECIMAL(5,2) -> integer_digits=3, max_integer=999, scale_factor=100
    // max_value = 999 * 100 + 99 = 99999
    // 但需要检查乘法是否溢出
    if (multiply_would_overflow(max_integer, scale_factor)) {
        // 如果乘法会溢出，则只检查是否在 int64_t 范围内
        return true;
    }
    
    std::int64_t max_scaled_integer = max_integer * scale_factor;
    std::int64_t max_fractional = scale_factor - 1;
    
    // 检查加法是否溢出
    if (max_scaled_integer > std::numeric_limits<std::int64_t>::max() - max_fractional) {
        return std::abs(value) <= std::numeric_limits<std::int64_t>::max();
    }
    
    std::int64_t max_value = max_scaled_integer + max_fractional;
    
    return std::abs(value) <= max_value;
}

int Decimal::align_scale(const Decimal & a, const Decimal & b)
{
    return std::max(a.scale_, b.scale_);
}

std::int64_t Decimal::scale_value(std::int64_t value, int from_scale, int to_scale)
{
    if (from_scale == to_scale) {
        return value;
    }
    
    if (to_scale > from_scale) {
        // 需要放大：乘以 10^(to_scale - from_scale)
        int diff = to_scale - from_scale;
        std::int64_t factor = power_of_10(diff);
        
        // 检查是否溢出
        if (multiply_would_overflow(value, factor)) {
            throw std::overflow_error("Decimal scale conversion overflow");
        }
        
        return value * factor;
    } else {
        // 需要缩小：除以 10^(from_scale - to_scale)
        // 这里使用整数除法，会丢失精度
        int diff = from_scale - to_scale;
        std::int64_t factor = power_of_10(diff);
        return value / factor;
    }
}

std::int64_t Decimal::parse_string(const std::string & str, int scale)
{
    if (str.empty()) {
        throw std::invalid_argument("Empty string cannot be parsed as Decimal");
    }
    
    // 去除前后空格
    std::string trimmed = str;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.empty()) {
        throw std::invalid_argument("Empty string cannot be parsed as Decimal");
    }
    
    // 查找小数点
    std::size_t dot_pos = trimmed.find('.');
    
    std::string integer_part_str;
    std::string fractional_part_str;
    
    if (dot_pos == std::string::npos) {
        // 没有小数点
        integer_part_str = trimmed;
        fractional_part_str = "";
    } else {
        integer_part_str = trimmed.substr(0, dot_pos);
        fractional_part_str = trimmed.substr(dot_pos + 1);
        
        // 检查是否有多余的小数点
        if (trimmed.find('.', dot_pos + 1) != std::string::npos) {
            throw std::invalid_argument("Invalid Decimal string format: multiple decimal points");
        }
    }
    
    // 处理符号
    bool negative = false;
    if (!integer_part_str.empty() && integer_part_str[0] == '-') {
        negative = true;
        integer_part_str = integer_part_str.substr(1);
    } else if (!integer_part_str.empty() && integer_part_str[0] == '+') {
        // 允许显式的正号
        integer_part_str = integer_part_str.substr(1);
    }
    
    // 检查是否为空（只有符号）
    if (integer_part_str.empty() && fractional_part_str.empty()) {
        throw std::invalid_argument("Invalid Decimal string format: only sign");
    }
    
    // 验证整数部分只包含数字
    if (!integer_part_str.empty() && 
        !std::all_of(integer_part_str.begin(), integer_part_str.end(), ::isdigit)) {
        throw std::invalid_argument("Invalid character in integer part");
    }
    
    // 验证小数部分只包含数字
    if (!fractional_part_str.empty() && 
        !std::all_of(fractional_part_str.begin(), fractional_part_str.end(), ::isdigit)) {
        throw std::invalid_argument("Invalid character in fractional part");
    }
    
    // 解析整数部分
    std::int64_t integer_part = 0;
    if (!integer_part_str.empty()) {
        try {
            integer_part = std::stoll(integer_part_str);
        } catch (const std::out_of_range &) {
            throw std::overflow_error("Integer part too large");
        } catch (const std::exception &) {
            throw std::invalid_argument("Invalid integer part");
        }
    }
    
    // 解析小数部分
    std::int64_t fractional_part = 0;
    if (!fractional_part_str.empty()) {
        // 截断或补零到 scale 位
        std::string frac_normalized = fractional_part_str;
        if (static_cast<int>(frac_normalized.length()) > scale) {
            // 截断到 scale 位（四舍五入可以考虑，但这里简单截断）
            frac_normalized = frac_normalized.substr(0, scale);
        } else if (static_cast<int>(frac_normalized.length()) < scale) {
            // 补零到 scale 位
            frac_normalized.append(scale - frac_normalized.length(), '0');
        }
        
        if (!frac_normalized.empty()) {
            try {
                fractional_part = std::stoll(frac_normalized);
            } catch (const std::out_of_range &) {
                throw std::overflow_error("Fractional part too large");
            } catch (const std::exception &) {
                throw std::invalid_argument("Invalid fractional part");
            }
        }
    }
    
    // 计算缩放后的值
    std::int64_t scale_factor = compute_scale_factor(scale);
    
    // 检查乘法是否溢出
    if (multiply_would_overflow(integer_part, scale_factor)) {
        throw std::overflow_error("Decimal value exceeds maximum");
    }
    
    std::int64_t scaled_integer = integer_part * scale_factor;
    
    // 检查加法是否溢出
    if (scaled_integer > 0 && fractional_part > std::numeric_limits<std::int64_t>::max() - scaled_integer) {
        throw std::overflow_error("Decimal value exceeds maximum");
    }
    if (scaled_integer < 0 && fractional_part < std::numeric_limits<std::int64_t>::min() - scaled_integer) {
        throw std::overflow_error("Decimal value below minimum");
    }
    
    std::int64_t result = scaled_integer + fractional_part;
    
    if (negative) {
        if (result == std::numeric_limits<std::int64_t>::min()) {
            // 已经是最小值，不能再取负
            throw std::overflow_error("Decimal value below minimum");
        }
        result = -result;
    }
    
    return result;
}

} // namespace dreamdb

