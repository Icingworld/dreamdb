#include "dreamdb/function/builtin/math/abs_function.h"

#include <cassert>
#include <stdexcept>
#include <cmath>

namespace dreamdb
{

AbsFunction::AbsFunction()
    : ScalarFunction("ABS", {
        LogicalType{LogicalTypeId::LOGICAL_TYPE_FLOAT, 0, false},
        LogicalType{LogicalTypeId::LOGICAL_TYPE_INTEGER, 0, false}
    })
{
}

LogicalType AbsFunction::return_type(const std::vector<LogicalType> & argument_types) const
{
    assert(argument_types.size() == 1);

    const auto & arg = argument_types[0];
    if (arg.id != LogicalTypeId::LOGICAL_TYPE_FLOAT && arg.id != LogicalTypeId::LOGICAL_TYPE_INTEGER) {
        throw std::invalid_argument("ABS function only supports float and integer types");
    }

    return LogicalType{arg.id, 0, arg.nullable};
}

std::optional<FieldValue> AbsFunction::execute(const std::vector<FieldValue> & args) const
{
    assert(args.size() == 1);

    const auto & arg = args[0];
    
    // NULL 传播
    if (std::holds_alternative<Null>(arg)) {
        return std::nullopt;
    }

    return std::visit(
        [](auto&& value) -> std::optional<FieldValue> {
            using T = std::decay_t<decltype(value)>;

            // 整型
            if constexpr (std::is_integral_v<T>) {
                return FieldValue{ value < 0 ? -value : value };
            }
            // 浮点
            else if constexpr (std::is_floating_point_v<T>) {
                return FieldValue{ std::fabs(value) };
            }
            // Decimal 暂不支持
            // 其他类型不应该出现
            else {
                throw std::invalid_argument("ABS: unsupported argument type");
            }
        },
        arg
    );
}

} // namespace dreamdb
