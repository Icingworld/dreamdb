#pragma once

#include <string>
#include <variant>
#include <cstdint>

#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 空值类型
 */
class NullType
{
public:
    NullType() = default;

    ~NullType() = default;
};

/**
 * @brief 字段默认值类型
 */
using FieldDefaultValue = std::variant<
    std::int8_t,                    // INT8
    std::int16_t,                   // INT16
    std::int32_t,                   // INT32
    std::int64_t,                   // INT64, TIMESTAMP
    float,                          // FLOAT
    double,                         // DOUBLE
    std::string,                    // STRING
    bool,                           // BOOLEAN
    NullType                        // NULL
>;

/**
 * @brief 字段定义
 */
class Field
{
public:
    Field(
        const std::string & name,
        FieldType type,
        int length,
        int precision,
        bool is_nullable,
        bool is_primary,
        const std::string & comment,
        const FieldDefaultValue & default_value,
        bool is_auto_increment,
        bool is_vector,
        int dimension,
        MetricType metric_type
    );

    ~Field() = default;

public:
    /**
     * @brief 验证字段是否合法
     * @return 字段是否合法
     */
    bool is_valid() const;

    /**
     * @brief 设置字段名称
     * @param name 字段名称
     */
    void set_name(const std::string & name);

    /**
     * @brief 设置字段类型
     * @param type 字段类型
     */
    void set_type(FieldType type);

    /**
     * @brief 设置字段长度
     * @param length 字段长度
     */
    void set_length(int length);

    /**
     * @brief 设置字段小数点位数
     * @param precision 字段小数点位数
     */
    void set_precision(int precision);

    /**
     * @brief 设置字段是否可为空
     * @param is_nullable 字段是否可为空
     */
    void set_is_nullable(bool is_nullable);

    /**
     * @brief 设置字段是否为主键
     * @param is_primary 字段是否为主键
     */
    void set_is_primary(bool is_primary);

    /**
     * @brief 设置字段注释
     * @param comment 字段注释
     */
    void set_comment(const std::string & comment);

    /**
     * @brief 设置字段默认值
     * @param default_value 字段默认值
     */
    void set_default_value(const FieldDefaultValue & default_value);

    /**
     * @brief 设置字段是否自动递增
     * @param is_auto_increment 字段是否自动递增
     */
    void set_is_auto_increment(bool is_auto_increment);

    /**
     * @brief 设置字段是否为向量字段
     * @param is_vector 字段是否为向量字段
     */
    void set_is_vector(bool is_vector);

    /**
     * @brief 设置向量维度
     * @param dimension 向量维度
     */
    void set_dimension(int dimension);

    /**
     * @brief 设置向量距离度量类型
     * @param metric_type 向量距离度量类型
     */
    void set_metric_type(MetricType metric_type);

    /**
     * @brief 获取字段名称
     * @return 字段名称
     */
    const std::string & get_name() const;

    /**
     * @brief 获取字段类型
     * @return 字段类型
     */
    FieldType get_type() const;

    /**
     * @brief 获取字段长度
     * @return 字段长度
     */
    int get_length() const;

    /**
     * @brief 获取字段小数点位数
     * @return 字段小数点位数
     */
    int get_precision() const;

    /**
     * @brief 获取字段是否可为空
     * @return 字段是否可为空
     */
    bool get_is_nullable() const;

    /**
     * @brief 获取字段是否为主键
     * @return 字段是否为主键
     */
    bool get_is_primary() const;

    /**
     * @brief 获取字段注释
     * @return 字段注释
     */
    const std::string & get_comment() const;

    /**
     * @brief 获取字段默认值
     * @return 字段默认值
     */
    const FieldDefaultValue & get_default_value() const;

    /**
     * @brief 获取字段是否自动递增
     * @return 字段是否自动递增
     */
    bool get_is_auto_increment() const;

    /**
     * @brief 获取字段是否为向量字段
     * @return 字段是否为向量字段
     */
    bool get_is_vector() const;

    /**
     * @brief 获取向量维度
     * @return 向量维度
     */
    int get_dimension() const;

    /**
     * @brief 获取向量距离度量类型
     * @return 向量距离度量类型
     */
    MetricType get_metric_type() const;

    /**
     * @brief 创建 INT64 字段
    */
    static Field create_int64_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 FLOAT 字段
    */
    static Field create_float_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 DOUBLE 字段
    */
    static Field create_double_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 CHAR 字段
    */
    static Field create_char_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 VARCHAR 字段
    */
    static Field create_varchar_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 STRING 字段
    */
    static Field create_string_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 BOOLEAN 字段
    */
    static Field create_boolean_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 TIMESTAMP 字段
    */
    static Field create_timestamp_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 ENUM 字段
    */
    static Field create_enum_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = false,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

    /**
     * @brief 创建 FLOAT_VECTOR 字段
    */
    static Field create_float_vector_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldDefaultValue & default_value = NullType(),
        bool is_auto_increment = false,
        bool is_vector = true,
        int dimension = 0,
        MetricType metric_type = MetricType::NONE
    );

private:
    // 基本属性
    std::string name;                   // 字段名称
    FieldType type;                     // 字段类型
    int length;                         // 字段长度
    int precision;                      // 字段小数点位数
    bool is_nullable;                   // 字段是否可为空
    bool is_primary;                    // 字段是否为主键
    std::string comment;                // 字段注释

    // 特定类型字段属性
    FieldDefaultValue default_value;    // 字段默认值
    bool is_auto_increment;             // 字段是否自动递增

    // 向量字段属性
    bool is_vector;                     // 字段是否为向量字段
    int dimension;                      // 向量维度
    MetricType metric_type;             // 向量距离度量类型
};

} // namespace dreamdb
