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
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
)
    : name(name),
      type(type),
      length(length),
      precision(precision),
      is_nullable(is_nullable),
      is_primary(is_primary),
      comment(comment),
      default_value(default_value),
      is_auto_increment(is_auto_increment),
      is_vector(is_vector),
      dimension(dimension),
      metric_type(metric_type)
{
}

bool Field::is_valid() const
{
    // TODO: 验证字段是否合法
    return true;
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
        case FieldType::INT64:
            break;
        case FieldType::FLOAT:
            break;
        case FieldType::DOUBLE:
            break;
        case FieldType::CHAR:
            break;
        case FieldType::VARCHAR:
            break;
        case FieldType::STRING:
            break;
        case FieldType::BOOLEAN:
            break;
        case FieldType::TIMESTAMP:
            break;
        case FieldType::ENUM:
            break;
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

void Field::set_default_value(const FieldDefaultValue & default_value)
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
            case FieldType::INT64:
                return std::is_same_v<T, std::int64_t>;
            case FieldType::FLOAT:
                return std::is_same_v<T, float>;
            case FieldType::DOUBLE:
                return std::is_same_v<T, double>;
            case FieldType::CHAR:
            case FieldType::VARCHAR:
            case FieldType::STRING:
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
            else if constexpr (std::is_same_v<T, std::int64_t>) {
                return "int64";
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
    if (type != FieldType::INT64) {
        throw std::invalid_argument("Auto increment is only supported for INT64 fields");
    }

    this->is_auto_increment = is_auto_increment;
}

void Field::set_is_vector(bool is_vector)
{
    this->is_vector = is_vector;
}

void Field::set_dimension(int dimension)
{
    if (type != FieldType::FLOAT_VECTOR) {
        throw std::invalid_argument("Dimension is only supported for FLOAT_VECTOR fields");
    }

    if (dimension <= 0) {
        throw std::invalid_argument("Dimension must be greater than 0");
    }

    this->dimension = dimension;
}

void Field::set_metric_type(MetricType metric_type)
{
    if (type != FieldType::FLOAT_VECTOR) {
        throw std::invalid_argument("Metric type is only supported for FLOAT_VECTOR fields");
    }

    this->metric_type = metric_type;
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

const FieldDefaultValue & Field::get_default_value() const
{
    return default_value;
}

bool Field::get_is_auto_increment() const
{
    return is_auto_increment;
}

bool Field::get_is_vector() const
{
    return is_vector;
}

int Field::get_dimension() const
{
    return dimension;
}

MetricType Field::get_metric_type() const
{
    return metric_type;
}

Field Field::create_int64_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_float_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_double_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_char_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_varchar_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_string_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
)
{
    return Field(
        name,
        FieldType::STRING,
        0,
        0,
        is_nullable,
        is_primary,
        comment,
        default_value,
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_boolean_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_timestamp_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_enum_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

Field Field::create_float_vector_field(
    const std::string & name,
    bool is_nullable,
    bool is_primary,
    const std::string & comment,
    const FieldDefaultValue & default_value,
    bool is_auto_increment,
    bool is_vector,
    int dimension,
    MetricType metric_type
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
        is_auto_increment,
        is_vector,
        dimension,
        metric_type
    );
}

} // namespace dreamdb
