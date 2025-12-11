#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

namespace dreamdb
{

/**
 * @brief 定点数类型
 * @details 用于表示 DECIMAL(p, s) 类型，其中 p 是总精度，s 是小数位数
 *          内部使用 int64_t 存储缩放后的整数值，避免浮点误差
 *          例如：DECIMAL(10, 2) 存储 123.45 时，value_ = 12345
 */
class Decimal
{
public:
    /**
     * @brief 构造函数：从缩放后的整数值构造
     * @param value 缩放后的整数值（已乘以 10^scale）
     * @param precision 总精度（整数部分 + 小数部分的最大位数）
     * @param scale 小数位数
     * @throw std::invalid_argument 如果 precision 或 scale 无效
     */
    Decimal(std::int64_t value, int precision, int scale);

    /**
     * @brief 构造函数：从 double 构造
     * @param value 浮点数值
     * @param precision 总精度
     * @param scale 小数位数
     * @throw std::invalid_argument 如果 precision 或 scale 无效
     * @throw std::overflow_error 如果值超出范围
     */
    Decimal(double value, int precision, int scale);

    /**
     * @brief 构造函数：从字符串构造
     * @param str 字符串表示（如 "123.45"）
     * @param precision 总精度
     * @param scale 小数位数
     * @throw std::invalid_argument 如果字符串格式无效或 precision/scale 无效
     * @throw std::overflow_error 如果值超出范围
     */
    Decimal(const std::string & str, int precision, int scale);

    /**
     * @brief 拷贝构造函数
     */
    Decimal(const Decimal & other) = default;

    /**
     * @brief 移动构造函数
     */
    Decimal(Decimal && other) noexcept = default;

    /**
     * @brief 拷贝赋值运算符
     */
    Decimal & operator=(const Decimal & other) = default;

    /**
     * @brief 移动赋值运算符
     */
    Decimal & operator=(Decimal && other) noexcept = default;

    ~Decimal() = default;

public:
    /**
     * @brief 获取缩放后的整数值
     * @return 缩放后的整数值
     */
    std::int64_t value() const noexcept;

    /**
     * @brief 获取总精度
     * @return 总精度
     */
    int precision() const noexcept;

    /**
     * @brief 获取小数位数
     * @return 小数位数
     */
    int scale() const noexcept;

    /**
     * @brief 转换为 double
     * @return double 值
     * @note 可能丢失精度
     */
    double to_double() const noexcept;

    /**
     * @brief 转换为字符串
     * @return 字符串表示（如 "123.45"）
     */
    std::string to_string() const;

public:
    /**
     * @brief 算术运算符：加法
     * @param other 另一个 Decimal
     * @return 结果（精度和 scale 取两者中较大的）
     * @throw std::overflow_error 如果结果溢出
     */
    Decimal operator+(const Decimal & other) const;

    /**
     * @brief 算术运算符：减法
     * @param other 另一个 Decimal
     * @return 结果（精度和 scale 取两者中较大的）
     * @throw std::overflow_error 如果结果溢出
     */
    Decimal operator-(const Decimal & other) const;

    /**
     * @brief 算术运算符：乘法
     * @param other 另一个 Decimal
     * @return 结果（精度为两者之和，scale 为两者之和）
     * @throw std::overflow_error 如果结果溢出
     */
    Decimal operator*(const Decimal & other) const;

    /**
     * @brief 算术运算符：除法
     * @param other 另一个 Decimal
     * @return 结果（精度和 scale 取被除数的值）
     * @throw std::domain_error 如果除数为零
     * @throw std::overflow_error 如果结果溢出
     */
    Decimal operator/(const Decimal & other) const;

    /**
     * @brief 算术运算符：取模
     * @param other 另一个 Decimal
     * @return 结果
     * @throw std::domain_error 如果除数为零
     */
    Decimal operator%(const Decimal & other) const;

    /**
     * @brief 一元运算符：取负
     * @return 负值
     */
    Decimal operator-() const;

public:
    /**
     * @brief 比较运算符：等于
     * @param other 另一个 Decimal
     * @return 是否相等
     */
    bool operator==(const Decimal & other) const;

    /**
     * @brief 比较运算符：不等于
     * @param other 另一个 Decimal
     * @return 是否不相等
     */
    bool operator!=(const Decimal & other) const;

    /**
     * @brief 比较运算符：小于
     * @param other 另一个 Decimal
     * @return 是否小于
     */
    bool operator<(const Decimal & other) const;

    /**
     * @brief 比较运算符：小于等于
     * @param other 另一个 Decimal
     * @return 是否小于等于
     */
    bool operator<=(const Decimal & other) const;

    /**
     * @brief 比较运算符：大于
     * @param other 另一个 Decimal
     * @return 是否大于
     */
    bool operator>(const Decimal & other) const;

    /**
     * @brief 比较运算符：大于等于
     * @param other 另一个 Decimal
     * @return 是否大于等于
     */
    bool operator>=(const Decimal & other) const;

private:
    /**
     * @brief 验证精度和 scale 是否有效
     * @param precision 总精度
     * @param scale 小数位数
     * @throw std::invalid_argument 如果无效
     */
    static void validate_precision_scale(int precision, int scale);

    /**
     * @brief 计算缩放因子（10^scale）
     * @param scale 小数位数
     * @return 缩放因子
     */
    static std::int64_t compute_scale_factor(int scale);

    /**
     * @brief 检查值是否在精度范围内
     * @param value 缩放后的值
     * @param precision 总精度
     * @param scale 小数位数
     * @return 是否在范围内
     */
    static bool is_in_range(std::int64_t value, int precision, int scale);

    /**
     * @brief 对齐两个 Decimal 的 scale（用于运算）
     * @param a 第一个 Decimal
     * @param b 第二个 Decimal
     * @return 对齐后的 scale
     */
    static int align_scale(const Decimal & a, const Decimal & b);

    /**
     * @brief 将值缩放到指定的 scale
     * @param value 原始值
     * @param from_scale 原始 scale
     * @param to_scale 目标 scale
     * @return 缩放后的值
     * @throw std::overflow_error 如果溢出
     */
    static std::int64_t scale_value(std::int64_t value, int from_scale, int to_scale);

    /**
     * @brief 从字符串解析 Decimal 值
     * @param str 字符串
     * @param scale 小数位数
     * @return 缩放后的整数值
     * @throw std::invalid_argument 如果格式无效
     * @throw std::overflow_error 如果值超出范围
     */
    static std::int64_t parse_string(const std::string & str, int scale);

private:
    std::int64_t value_;    // 缩放后的整数值（已乘以 10^scale）
    int precision_;         // 总精度（整数部分 + 小数部分的最大位数）
    int scale_;             // 小数位数
};

} // namespace dreamdb
