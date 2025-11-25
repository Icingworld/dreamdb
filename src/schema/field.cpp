#include "dreamdb/schema/field.h"

#include <stdexcept>
#include <type_traits>

namespace dreamdb
{

Field::Field(
    const std::string & name,
    FieldType type,
    int length,
    int precision,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value,
    bool is_auto_increment
)
    : name(name)
    , type(type)
    , length(length)
    , precision(precision)
    , is_nullable(is_nullable)
    , is_primary(is_primary)
    , comment(comment)
    , default_value(default_value)
    , is_auto_increment(is_auto_increment)
{
}

void Field::set_name(const std::string & name)
{
    this->name = name;
}

void Field::set_type(FieldType type)
{
    this->type = type;

    // TODO: 根据类型做出不同的初始化处理
    switch (type)
    {
        case FieldType::INT8:
        case FieldType::INT16:
        case FieldType::INT32:
        case FieldType::INT64:
        case FieldType::FLOAT:
        case FieldType::DOUBLE:
        case FieldType::CHAR:
        case FieldType::VARCHAR:
        case FieldType::BOOLEAN:
        case FieldType::TIMESTAMP:
        case FieldType::ENUM:
        case FieldType::FLOAT_VECTOR:
            break;
        default:
            throw std::invalid_argument("Invalid field type");
    }
}

void Field::set_length(int length)
{
    this->length = length;
}

void Field::set_precision(int precision)
{
    this->precision = precision;
}

void Field::set_is_nullable(bool is_nullable)
{
    this->is_nullable = is_nullable;
}

void Field::set_is_primary(bool is_primary)
{
    this->is_primary = is_primary;
}

void Field::set_comment(const std::string & comment)
{
    this->comment = comment;
}

void Field::set_default_value(const FieldValue & default_value)
{
    // 检查是否为 NullType（允许空值）
    if (std::holds_alternative<NullType>(default_value)) {
        this->default_value = default_value;
        return;
    }

    // 向量类型和枚举类型不支持默认值
    if (type == FieldType::FLOAT_VECTOR || type == FieldType::ENUM) {
        throw std::invalid_argument(
            "Default value is not supported for field type " + 
            std::to_string(static_cast<int>(type))
        );
    }

    // 验证默认值类型是否与字段类型匹配
    bool is_valid = std::visit([this](const auto & value) -> bool {
        using T = std::decay_t<decltype(value)>;

        switch (type) {
            case FieldType::INT8:
                return std::is_same_v<T, std::int8_t>;
            case FieldType::INT16:
                return std::is_same_v<T, std::int16_t>;
            case FieldType::INT32:
                return std::is_same_v<T, std::int32_t>;
            case FieldType::INT64:
                return std::is_same_v<T, std::int64_t>;
            case FieldType::FLOAT:
                return std::is_same_v<T, float>;
            case FieldType::DOUBLE:
                return std::is_same_v<T, double>;
            case FieldType::CHAR:
                return std::is_same_v<T, char>;
            case FieldType::VARCHAR:
                return std::is_same_v<T, std::string>;
            case FieldType::BOOLEAN:
                return std::is_same_v<T, bool>;
            case FieldType::TIMESTAMP:
                return std::is_same_v<T, std::int64_t>;
            default:
                return false;
        }
    }, default_value);

    if (!is_valid) {
        // 获取默认值的类型名称用于错误提示
        std::string value_type_name = std::visit([](const auto & value) -> std::string {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::int8_t>) {
                return "int8";
            }
            else if constexpr (std::is_same_v<T, std::int16_t>) {
                return "int16";
            }
            else if constexpr (std::is_same_v<T, std::int32_t>) {
                return "int32";
            }
            else if constexpr (std::is_same_v<T, std::int64_t>) {
                return "int64";
            }
            else if constexpr (std::is_same_v<T, float>) {
                return "float";
            }
            else if constexpr (std::is_same_v<T, double>) {
                return "double";
            }
            else if constexpr (std::is_same_v<T, std::string>) {
                return "string";
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return "bool";
            }
            else if constexpr (std::is_same_v<T, char>) {
                return "char";
            }
            else {
                return "unknown";
            }
        }, default_value);

        throw std::invalid_argument(
            "Default value type (" + value_type_name + ") does not match field type " +
            std::to_string(static_cast<int>(type))
        );
    }

    this->default_value = default_value;
}

void Field::set_is_auto_increment(bool is_auto_increment)
{
    // 自增仅支持整数类型，后续可能添加
    if (type != FieldType::INT8 && type != FieldType::INT16 && type != FieldType::INT32 && type != FieldType::INT64) {
        throw std::invalid_argument("Auto increment is only supported for integer fields");
    }

    this->is_auto_increment = is_auto_increment;
}

const std::string & Field::get_name() const
{
    return name;
}

FieldType Field::get_type() const
{
    return type;    
}

int Field::get_length() const
{
    return length;
}

int Field::get_precision() const
{
    return precision;
}

bool Field::get_is_nullable() const
{
    return is_nullable;
}

bool Field::get_is_primary() const
{
    return is_primary;
}

const std::string & Field::get_comment() const
{
    return comment;
}

const FieldValue & Field::get_default_value() const
{
    return default_value;
}

bool Field::get_is_auto_increment() const
{
    return is_auto_increment;
}

Field Field::create_int8_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value,
    bool is_auto_increment
)
{
    return Field(
        name,
        FieldType::INT8,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment
    );
}

Field Field::create_int16_field(

    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value,
    bool is_auto_increment
)
{
    return Field(
        name,
        FieldType::INT16,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment
    );
}

Field Field::create_int32_field(

    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value,
    bool is_auto_increment
)
{
    return Field(
        name,
        FieldType::INT32,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment
    );
}

Field Field::create_int64_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value,
    bool is_auto_increment
)
{
    return Field(
        name,
        FieldType::INT64,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment
    );
}

Field Field::create_float_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::FLOAT,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_double_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::DOUBLE,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_char_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::CHAR,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_varchar_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::VARCHAR,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_boolean_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::BOOLEAN,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_timestamp_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::TIMESTAMP,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_enum_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::ENUM,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_float_vector_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::FLOAT_VECTOR,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

} // namespace dreamdb
