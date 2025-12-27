#include "dreamdb/evaluator/evaluator.h"

#include "dreamdb/parser/ast/literal_expr.h"

namespace dreamdb
{

EvaluateResult::EvaluateResult() noexcept
    : value_(Null{})
    , is_success_(false)
    , error_message_()
{
}

EvaluateResult EvaluateResult::make_success(const FieldValue & value)
{
    EvaluateResult result;
    result.set_value(value);
    result.set_is_success(true);
    return result;
}

EvaluateResult EvaluateResult::make_error(const std::string & error_message)
{
    EvaluateResult result;
    result.set_error_message(error_message);
    return result;
}

void EvaluateResult::set_value(const FieldValue & value)
{
    value_ = value;
}

void EvaluateResult::set_is_success(bool is_success) noexcept
{
    is_success_ = is_success;
}

void EvaluateResult::set_error_message(const std::string & error_message)
{
    error_message_ = error_message;
}

const FieldValue & EvaluateResult::get_value() const noexcept
{
    return value_;
}

bool EvaluateResult::get_is_success() const noexcept
{
    return is_success_;
}

const std::string & EvaluateResult::get_error_message() const noexcept
{
    return error_message_;
}

EvaluateResult Evaluator::evaluate(
    const AstNode * expr,
    const EvaluatorContext & context,
    std::optional<FieldType> field_type
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    // 根据表达式类型选择对应的评估方法
    EvaluateResult result;
    switch (expr->get_type()) {
        case AstNodeType::LITERAL_EXPR:
            return evaluate_literal(expr, field_type);
            break;
        case AstNodeType::IDENTIFIER_EXPR:
            return evaluate_identifier(expr, context);
            break;
        case AstNodeType::BINARY_EXPR:
            return evaluate_binary(expr, context);
            break;
        case AstNodeType::UNARY_EXPR:
            return evaluate_unary(expr, context);
            break;
        case AstNodeType::LIKE_EXPR:
            return evaluate_like(expr, context);
            break;
        case AstNodeType::IN_EXPR:
            return evaluate_in(expr, context);
            break;
        case AstNodeType::BETWEEN_EXPR:
            return evaluate_between(expr, context);
            break;
        case AstNodeType::NULL_EXPR:
            return evaluate_null(expr, context);
            break;
        case AstNodeType::FUNCTION_CALL_EXPR:
            return evaluate_function_call(expr, context);
            break;
        default:
            return EvaluateResult::make_error("Unsupported expression type: " + std::to_string(static_cast<std::uint8_t>(expr->get_type())));
    }

    // 如果评估失败，直接返回
    if (!result.get_is_success()) {
        return result;
    }

    // 如果评估成功且有目标类型，进行类型转换
    // TODO: 实现类型转换逻辑
    // if (field_type.has_value()) {
    //     result = convert_type(result, field_type.value());
    // }

    return result;
}

std::optional<bool> Evaluator::evaluate_condition(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        // WHERE 子句为空，条件为真，此时匹配所有记录
        return true;
    }

    // 递归评估表达式
    EvaluateResult result = evaluate(expr, context);

    // 如果评估失败，返回 nullopt 表示出现错误
    if (!result.get_is_success()) {
        return std::nullopt;
    }

    // 如果评估成功，转换为 bool 值
    const FieldValue & value = result.get_value();
    bool bool_result = field_value_to_bool(value);

    return bool_result;
}

EvaluateResult Evaluator::evaluate_literal(
    const AstNode * expr,
    std::optional<FieldType> target_type
) const
{
    if (expr == nullptr) {
        // 字面量表达式为空，返回错误
        return EvaluateResult::make_error("Expression is null");
    }

    if (expr->get_type() != AstNodeType::LITERAL_EXPR) {
        // 不是字面量表达式，返回错误
        return EvaluateResult::make_error("Invalid literal expression type: " + std::to_string(static_cast<std::uint8_t>(expr->get_type())));
    }

    // 获取字面量表达式
    const LiteralExpr * literal = static_cast<const LiteralExpr *>(expr);
    LiteralExpr::LiteralType literal_type = literal->get_literal_type();
    const LiteralExpr::LiteralValue & literal_value = literal->get_literal_value();
    
    // 根据字面量类型和目标类型进行转换
    switch (literal_type) {
        case LiteralExpr::LiteralType::INTEGER: {
            // 读取整数值
            std::int64_t int_val = std::get<std::int64_t>(literal_value);

            // 如果有目标类型，进行类型转换
            if (target_type.has_value()) {
                switch (target_type.value()) {
                    case FieldType::TINYINT:
                        return EvaluateResult::make_success(FieldValue(static_cast<std::int8_t>(int_val)));
                    case FieldType::SMALLINT:
                        return EvaluateResult::make_success(FieldValue(static_cast<std::int16_t>(int_val)));
                    case FieldType::INTEGER:
                        return EvaluateResult::make_success(FieldValue(static_cast<std::int32_t>(int_val)));
                    case FieldType::BIGINT:
                    case FieldType::TIMESTAMP:
                        return EvaluateResult::make_success(FieldValue(int_val));
                    case FieldType::FLOAT:
                        return EvaluateResult::make_success(FieldValue(static_cast<float>(int_val)));
                    case FieldType::DOUBLE:
                        return EvaluateResult::make_success(FieldValue(static_cast<double>(int_val)));
                    default:
                        // 对于其他类型，返回 BIGINT
                        return EvaluateResult::make_success(FieldValue(int_val));
                }
            } else {
                // 没有目标类型，默认返回 BIGINT
                return EvaluateResult::make_success(FieldValue(int_val));
            }
        }
        case LiteralExpr::LiteralType::FLOAT: {
            // 读取浮点数值
            double float_val = std::get<double>(literal_value);

            // 如果有目标类型，进行类型转换
            if (target_type.has_value()) {
                switch (target_type.value()) {
                    case FieldType::FLOAT:
                        return EvaluateResult::make_success(FieldValue(static_cast<float>(float_val)));
                    case FieldType::DOUBLE:
                        return EvaluateResult::make_success(FieldValue(float_val));
                    case FieldType::TINYINT:
                        return EvaluateResult::make_success(FieldValue(static_cast<std::int8_t>(float_val)));
                    case FieldType::SMALLINT:
                        return EvaluateResult::make_success(FieldValue(static_cast<std::int16_t>(float_val)));
                    case FieldType::INTEGER:
                        return EvaluateResult::make_success(FieldValue(static_cast<std::int32_t>(float_val)));
                    case FieldType::BIGINT:
                        return EvaluateResult::make_success(FieldValue(static_cast<std::int64_t>(float_val)));
                    default:
                        return EvaluateResult::make_success(FieldValue(float_val));
                }
            } else {
                // 没有目标类型，默认返回 DOUBLE
                return EvaluateResult::make_success(FieldValue(float_val));
            }
        }
        case LiteralExpr::LiteralType::STRING: {
            const std::string & str_val = std::get<std::string>(literal_value);
            // 字符串直接返回，适用于 CHAR, VARCHAR, ENUM
            return EvaluateResult::make_success(FieldValue(str_val));
        }
        case LiteralExpr::LiteralType::BOOLEAN: {
            bool bool_val = std::get<bool>(literal_value);
            return EvaluateResult::make_success(FieldValue(bool_val));
        }
        case LiteralExpr::LiteralType::NULL_VALUE: {
            return EvaluateResult::make_success(FieldValue(Null()));
        }
        case LiteralExpr::LiteralType::VECTOR: {
            const std::vector<float> & vec_val = std::get<std::vector<float>>(literal_value);
            return EvaluateResult::make_success(FieldValue(vec_val));
        }
        default:
            return EvaluateResult::make_error("Unsupported literal type: " + std::to_string(static_cast<std::uint8_t>(literal_type)));
    }
}

EvaluateResult Evaluator::evaluate_identifier(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

EvaluateResult Evaluator::evaluate_binary(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

EvaluateResult Evaluator::evaluate_unary(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

EvaluateResult Evaluator::evaluate_like(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

EvaluateResult Evaluator::evaluate_in(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

EvaluateResult Evaluator::evaluate_between(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

EvaluateResult Evaluator::evaluate_null(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

EvaluateResult Evaluator::evaluate_function_call(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }
}

bool Evaluator::field_value_to_bool(const FieldValue & value) const
{
    return std::visit([](const auto & v) -> bool {
        using T = std::decay_t<decltype(v)>;

        // NULL 值视为 false
        if constexpr (std::is_same_v<T, Null>) {
            return false;
        }
        // 布尔值直接返回
        else if constexpr (std::is_same_v<T, bool>) {
            return v;
        }
        // 整数类型：非零为真
        else if constexpr (std::is_integral_v<T>) {
            return v != 0;
        }
        // 浮点数类型：非零为真
        else if constexpr (std::is_floating_point_v<T>) {
            return v != 0.0;
        }
        // Decimal 类型：非零为真
        else if constexpr (std::is_same_v<T, Decimal>) {
            return v != Decimal(0.0, 0, 0);
        }
        // 字符串类型：非空为真
        else if constexpr (std::is_same_v<T, std::string>) {
            return !v.empty();
        }
        // 向量类型：非空为真
        else if constexpr (std::is_same_v<T, std::vector<float>>) {
            return !v.empty();
        }
        // 其他类型默认返回 false
        else {
            return false;
        }
    }, value);
}

} // namespace dreamdb
