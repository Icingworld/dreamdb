#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <vector>
#include <optional>

#include "dreamdb/common/type.h"
#include "dreamdb/common/decimal.h"
#include "dreamdb/common/null.h"

namespace dreamdb
{

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
    Decimal,                        // DECIMAL
    std::string,                    // CHAR, VARCHAR, ENUM
    bool,                           // BOOLEAN
    std::vector<float>,             // VECTOR
    Null                            // NULL
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
        const std::vector<std::string> & options,
        bool is_nullable,
        bool is_primary,
        const std::string & comment,
        const FieldValue & default_value,
        bool is_auto_increment
    );

    Field(const Field & other) = default;

    Field(Field && other) noexcept = default;

    Field & operator=(const Field & other) = default;

    Field & operator=(Field && other) noexcept = default;

    ~Field() = default;

public:
    /** 属性值访问接口 */

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
     * @brief 设置字段选项
     * @param options ENUM 字段选项
     */
    void set_options(const std::vector<std::string> & options);

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
    void set_default_value(const FieldValue & default_value);

    /**
     * @brief 设置字段是否自动递增
     * @param is_auto_increment 字段是否自动递增
     */
    void set_is_auto_increment(bool is_auto_increment);

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
     * @brief 获取字段选项
     * @return 字段选项
     */
    const std::vector<std::string> & get_options() const;

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
    const FieldValue & get_default_value() const;

    /**
     * @brief 获取字段是否自动递增
     * @return 字段是否自动递增
     */
    bool get_is_auto_increment() const;

public:
    /** 静态创建接口 */

    /**
     * @brief 创建 TINYINT 字段
     */
    static Field create_tinyint_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null(),
        bool is_auto_increment = false
    );

    /**
     * @brief 创建 SMALLINT 字段
     */
    static Field create_smallint_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null(),
        bool is_auto_increment = false
    );

    /**
     * @brief 创建 INTEGER 字段
     */
    static Field create_integer_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null(),
        bool is_auto_increment = false
    );

    /**
     * @brief 创建 BIGINT 字段
     */
    static Field create_bigint_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null(),
        bool is_auto_increment = false
    );

    /**
     * @brief 创建 FLOAT 字段
     */
    static Field create_float_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 DOUBLE 字段
     */
    static Field create_double_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 DECIMAL 字段
     */
    static Field create_decimal_field(
        const std::string & name = "",
        int precision = 0,
        int scale = 0,
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 CHAR 字段
     */
    static Field create_char_field(
        const std::string & name = "",
        int length = 1,
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 VARCHAR 字段
     */
    static Field create_varchar_field(
        const std::string & name = "",
        int length = 1,
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 BOOLEAN 字段
     */
    static Field create_boolean_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 TIMESTAMP 字段
     */
    static Field create_timestamp_field(
        const std::string & name = "",
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 ENUM 字段
     */
    static Field create_enum_field(
        const std::string & name = "",
        const std::vector<std::string> & enum_options = {},
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

    /**
     * @brief 创建 VECTOR 字段
     */
    static Field create_vector_field(
        const std::string & name = "",
        int dimension = 0,
        bool is_nullable = true,
        bool is_primary = false,
        const std::string & comment = "",
        const FieldValue & default_value = Null()
    );

private:
    // 基本属性
    std::string name_;                                      // 字段名称
    FieldType type_;                                        // 字段类型
    int length_;                                            // 字段长度
    int precision_;                                         // 字段小数点位数
    std::optional<std::vector<std::string>> options_;       // ENUM 字段选项
    bool is_nullable_;                                      // 字段是否可为空
    bool is_primary_;                                       // 字段是否为主键
    std::string comment_;                                   // 字段注释

    // 特定类型字段属性
    FieldValue default_value_;                              // 字段默认值
    bool is_auto_increment_;                                // 字段是否自动递增
};

} // namespace dreamdb
