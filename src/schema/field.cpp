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
    const std::vector<std::string> & options,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value,
    bool is_auto_increment
)
    : name_(name)
    , type_(type)
    , length_(length)
    , precision_(precision)
    , is_nullable_(is_nullable)
    , is_primary_(is_primary)
    , comment_(comment)
    , default_value_(default_value)
    , is_auto_increment_(is_auto_increment)
{
    if (options.empty()) {
        options_ = std::nullopt;
    } else {
        options_ = options;
    }
}

void Field::set_name(const std::string & name)
{
    name_ = name;
}

void Field::set_type(FieldType type)
{
    type_ = type;

    // TODO: 根据类型做出不同的初始化处理
    switch (type)
    {
        case FieldType::TINYINT:
        case FieldType::SMALLINT:
        case FieldType::INTEGER:
        case FieldType::BIGINT:
        case FieldType::FLOAT:
        case FieldType::DOUBLE:
        case FieldType::DECIMAL:
        case FieldType::CHAR:
        case FieldType::VARCHAR:
        case FieldType::BOOLEAN:
        case FieldType::TIMESTAMP:
        case FieldType::ENUM:
        case FieldType::VECTOR:
            break;
        default:
            throw std::invalid_argument("Invalid field type");
    }
}

void Field::set_length(int length)
{
    length_ = length;
}

void Field::set_precision(int precision)
{
    precision_ = precision;
}

void Field::set_options(const std::vector<std::string> & options)
{
    if (type_ != FieldType::ENUM) {
        throw std::invalid_argument("Options are only supported for enum fields");
    }

    options_ = options;
}

void Field::set_is_nullable(bool is_nullable)
{
    is_nullable_ = is_nullable;
}

void Field::set_is_primary(bool is_primary)
{
    is_primary_ = is_primary;
}

void Field::set_comment(const std::string & comment)
{
    comment_ = comment;
}

void Field::set_default_value(const FieldValue & default_value)
{
    // 检查是否为 Null（允许空值）
    if (std::holds_alternative<Null>(default_value)) {
        default_value_ = default_value;
        return;
    }

    // 向量类型和枚举类型不支持默认值
    if (type_ == FieldType::VECTOR || type_ == FieldType::ENUM) {
        throw std::invalid_argument(
            "Default value is not supported for field type " + 
            std::to_string(static_cast<int>(type_))
        );
    }

    // 验证默认值类型是否与字段类型匹配
    bool is_valid = std::visit([this](const auto & value) -> bool {
        using T = std::decay_t<decltype(value)>;

        switch (type_) {
            case FieldType::TINYINT:
                return std::is_same_v<T, std::int8_t>;
            case FieldType::SMALLINT:
                return std::is_same_v<T, std::int16_t>;
            case FieldType::INTEGER:
                return std::is_same_v<T, std::int32_t>;
            case FieldType::BIGINT:
                return std::is_same_v<T, std::int64_t>;
            case FieldType::FLOAT:
                return std::is_same_v<T, float>;
            case FieldType::DOUBLE:
                return std::is_same_v<T, double>;
            case FieldType::DECIMAL:
                return std::is_same_v<T, Decimal>;
            case FieldType::CHAR:
                return std::is_same_v<T, std::string>;
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
                return "tinyint";
            } else if constexpr (std::is_same_v<T, std::int16_t>) {
                return "smallint";
            } else if constexpr (std::is_same_v<T, std::int32_t>) {
                return "integer";
            } else if constexpr (std::is_same_v<T, std::int64_t>) {
                return "bigint";
            } else if constexpr (std::is_same_v<T, float>) {
                return "float";
            } else if constexpr (std::is_same_v<T, double>) {
                return "double";
            } else if constexpr (std::is_same_v<T, Decimal>) {
                return "decimal";
            } else if constexpr (std::is_same_v<T, std::string>) {
                return "string";
            } else if constexpr (std::is_same_v<T, bool>) {
                return "bool";
            } else {
                return "unknown";
            }
        }, default_value);

        throw std::invalid_argument(
            "Default value type (" + value_type_name + ") does not match field type " +
            std::to_string(static_cast<int>(type_))
        );
    }

    default_value_ = default_value;
}

void Field::set_is_auto_increment(bool is_auto_increment)
{
    // 自增仅支持整数类型，后续可能添加
    if (type_ != FieldType::TINYINT && type_ != FieldType::SMALLINT && type_ != FieldType::INTEGER && type_ != FieldType::BIGINT) {
        throw std::invalid_argument("Auto increment is only supported for integer fields");
    }

    is_auto_increment_ = is_auto_increment;
}

const std::string & Field::get_name() const
{
    return name_;
}

FieldType Field::get_type() const
{
    return type_;    
}

int Field::get_length() const
{
    return length_;
}

int Field::get_precision() const
{
    return precision_;
}

const std::vector<std::string> & Field::get_options() const
{
    if (type_ != FieldType::ENUM) {
        throw std::invalid_argument("Options are only supported for enum fields");
    }

    return options_.value();
}

bool Field::get_is_nullable() const
{
    return is_nullable_;
}

bool Field::get_is_primary() const
{
    return is_primary_;
}

const std::string & Field::get_comment() const
{
    return comment_;
}

const FieldValue & Field::get_default_value() const
{
    return default_value_;
}

bool Field::get_is_auto_increment() const
{
    return is_auto_increment_;
}

Field Field::create_tinyint_field(
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
        FieldType::TINYINT,
        0,
        0,
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment
    );
}

Field Field::create_smallint_field(

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
        FieldType::SMALLINT,
        0,
        0,
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment
    );
}

Field Field::create_integer_field(

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
        FieldType::INTEGER,
        0,
        0,
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment
    );
}

Field Field::create_bigint_field(
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
        FieldType::BIGINT,
        0,
        0,
        {},
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
        {},
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
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_char_field(
    const std::string & name,
    int length,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::CHAR,
        length,
        0,
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_varchar_field(
    const std::string & name,
    int length,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::VARCHAR,
        length,
        0,
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_decimal_field(
    const std::string & name,
    int precision,
    int scale,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::DECIMAL,
        precision,      // 作为 length 传入
        scale,       // 作为 precision 传入
        {},
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
        {},
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
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_enum_field(
    const std::string & name,
    const std::vector<std::string> & enum_options,
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
        enum_options,
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

Field Field::create_vector_field(
    const std::string & name,
    int dimension,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldValue & default_value
)
{
    return Field(
        name,
        FieldType::VECTOR,
        dimension,
        0,
        {},
        is_nullable,
        is_primary,
        comment,
        default_value,
        false
    );
}

} // namespace dreamdb
