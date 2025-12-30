#include "dreamdb/evaluator/evaluator.h"

#include "dreamdb/parser/ast/literal_expr.h"
#include "dreamdb/parser/ast/identifier_expr.h"
#include "dreamdb/parser/ast/binary_expr.h"

namespace dreamdb
{

// 辅助函数：将数值类型转换为 double
namespace {
    template<typename T>
    double to_double_value(const T & value) {
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            return static_cast<double>(value);
        } else if constexpr (std::is_same_v<T, Decimal>) {
            return value.to_double();
        } else {
            // 不应该到达这里
            return 0.0;
        }
    }
    
    template<typename T>
    std::int64_t to_int64_value(const T & value) {
        if constexpr (std::is_integral_v<T>) {
            return static_cast<std::int64_t>(value);
        } else {
            // 不应该到达这里
            return 0;
        }
    }
}

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
    // 检查表达式是否为空
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    // 检查表达式类型
    if (expr->get_type() != AstNodeType::LITERAL_EXPR) {
        return EvaluateResult::make_error("Invalid literal expression type: " + std::to_string(static_cast<std::uint8_t>(expr->get_type())));
    }

    // 获取字面量表达式
    const LiteralExpr * literal = static_cast<const LiteralExpr *>(expr);
    // 获取字面量类型和值
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
    // 检查表达式是否为空
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    // 检查表达式类型
    if (expr->get_type() != AstNodeType::IDENTIFIER_EXPR) {
        return EvaluateResult::make_error("Invalid identifier expression type: " + std::to_string(static_cast<std::uint8_t>(expr->get_type())));
    }

    // 检查上下文是否有效
    if (!context.is_valid()) {
        return EvaluateResult::make_error("Invalid evaluator context: entity or collection is null");
    }

    // 获取标识符表达式
    const IdentifierExpr * identifier = static_cast<const IdentifierExpr *>(expr);
    // 获取标识符类型和名称
    IdentifierExpr::IdentifierType identifier_type = identifier->get_identifier_type();
    const std::string & field_name = identifier->get_original_identifier();

    // 根据标识符类型和名称获取字段值
    switch (identifier_type) {
        // TODO: 支持集合名、别名、函数名
        case IdentifierExpr::IdentifierType::COLUMN: {
            // 从上下文中获取集合，查找字段
            const Collection * collection = context.get_collection();
            auto field_index = collection->get_field_index(field_name);

            if (!field_index.has_value()) {
                return EvaluateResult::make_error("Unknown field: " + field_name);
            }

            // 从 Entity 中读取字段值
            const Entity * entity = context.get_entity();
            try {
                const FieldValue & field_value = entity->get_value(field_index.value());
                return EvaluateResult::make_success(field_value);
            } catch (const std::out_of_range & e) {
                return EvaluateResult::make_error("Field index out of range: " + std::string(e.what()));
            }
        }
        default:
            return EvaluateResult::make_error("Unsupported identifier type: " + std::to_string(static_cast<std::uint8_t>(identifier_type)));
    }
}

EvaluateResult Evaluator::evaluate_binary(
    const AstNode * expr,
    const EvaluatorContext & context
) const
{
    // 检查表达式是否为空
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    // 检查表达式类型
    if (expr->get_type() != AstNodeType::BINARY_EXPR) {
        return EvaluateResult::make_error("Invalid binary expression type: " + std::to_string(static_cast<std::uint8_t>(expr->get_type())));
    }

    // 获取二元表达式
    const BinaryExpr * binary = static_cast<const BinaryExpr *>(expr);
    // 获取二元表达式类型和操作数
    BinaryExpr::OperatorType operator_type = binary->get_operator_type();
    const AstNode * left_expr = binary->get_left();
    const AstNode * right_expr = binary->get_right();

    // 递归评估左右操作数
    EvaluateResult left_result = evaluate(left_expr, context);
    EvaluateResult right_result = evaluate(right_expr, context);

    // 如果左右操作数评估失败，返回错误
    if (!left_result.get_is_success() || !right_result.get_is_success()) {
        return EvaluateResult::make_error("Failed to evaluate binary expression: " + left_result.get_error_message() + " or " + right_result.get_error_message());
    }

    // 如果左右操作数评估成功，根据运算符类型进行计算
    const FieldValue & left_value = left_result.get_value();
    const FieldValue & right_value = right_result.get_value();

    // 根据运算符类型进行计算
    switch (operator_type) {
        // 比较运算符
        case BinaryExpr::OperatorType::DB_EQUAL:
            return EvaluateResult::make_success(FieldValue(compare_values_equal(left_value, right_value)));
        case BinaryExpr::OperatorType::DB_NOT_EQUAL:
            return EvaluateResult::make_success(FieldValue(!compare_values_equal(left_value, right_value)));
        case BinaryExpr::OperatorType::DB_GREATER_THAN: {
            auto cmp = compare_values(left_value, right_value);
            if (cmp.has_value()) {
                return EvaluateResult::make_success(FieldValue(cmp.value() > 0));
            } else {
                return EvaluateResult::make_error("Cannot compare values of incompatible types");
            }
        }
        case BinaryExpr::OperatorType::DB_LESS_THAN: {
            auto cmp = compare_values(left_value, right_value);
            if (cmp.has_value()) {
                return EvaluateResult::make_success(FieldValue(cmp.value() < 0));
            } else {
                return EvaluateResult::make_error("Cannot compare values of incompatible types");
            }
        }
        case BinaryExpr::OperatorType::DB_GREATER_EQUAL: {
            auto cmp = compare_values(left_value, right_value);
            if (cmp.has_value()) {
                return EvaluateResult::make_success(FieldValue(cmp.value() >= 0));
            } else {
                return EvaluateResult::make_error("Cannot compare values of incompatible types");
            }
        }
        case BinaryExpr::OperatorType::DB_LESS_EQUAL: {
            auto cmp = compare_values(left_value, right_value);
            if (cmp.has_value()) {
                return EvaluateResult::make_success(FieldValue(cmp.value() <= 0));
            } else {
                return EvaluateResult::make_error("Cannot compare values of incompatible types");
            }
        }
        // 逻辑运算符
        case BinaryExpr::OperatorType::DB_AND: {
            bool left = field_value_to_bool(left_value);
            bool right = field_value_to_bool(right_value);
            return EvaluateResult::make_success(FieldValue(left && right));
        }
        case BinaryExpr::OperatorType::DB_OR: {
            bool left = field_value_to_bool(left_value);
            bool right = field_value_to_bool(right_value);
            return EvaluateResult::make_success(FieldValue(left || right));
        }
        // TODO: 算数运算符
        case BinaryExpr::OperatorType::DB_PLUS:
        case BinaryExpr::OperatorType::DB_MINUS:
        case BinaryExpr::OperatorType::DB_MULTIPLY:
        case BinaryExpr::OperatorType::DB_DIVIDE:
        case BinaryExpr::OperatorType::DB_MODULO:
            return EvaluateResult::make_error("Arithmetic operators not yet supported");
        default:
            return EvaluateResult::make_error("Unsupported binary operator: " + std::to_string(static_cast<std::uint8_t>(operator_type)));
    }
}

EvaluateResult Evaluator::evaluate_unary(
    const AstNode * expr,
    const EvaluatorContext &
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    return EvaluateResult::make_error("Unary operator not yet supported");
}

EvaluateResult Evaluator::evaluate_like(
    const AstNode * expr,
    const EvaluatorContext &
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    return EvaluateResult::make_error("Like operator not yet supported");
}

EvaluateResult Evaluator::evaluate_in(
    const AstNode * expr,
    const EvaluatorContext &
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    return EvaluateResult::make_error("In operator not yet supported");
}

EvaluateResult Evaluator::evaluate_between(
    const AstNode * expr,
    const EvaluatorContext &
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    return EvaluateResult::make_error("Between operator not yet supported");
}

EvaluateResult Evaluator::evaluate_null(
    const AstNode * expr,
    const EvaluatorContext &
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    return EvaluateResult::make_error("Null operator not yet supported");
}

EvaluateResult Evaluator::evaluate_function_call(
    const AstNode * expr,
    const EvaluatorContext &
) const
{
    if (expr == nullptr) {
        return EvaluateResult::make_error("Expression is null");
    }

    return EvaluateResult::make_error("Function call not yet supported");
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

bool Evaluator::compare_values_equal(const FieldValue & left, const FieldValue & right) const
{
    // 如果都是 NULL，则相等
    if (std::holds_alternative<Null>(left) && std::holds_alternative<Null>(right)) {
        return true;
    }

    // 如果一个是 NULL，一个是非 NULL，则不相等
    if (std::holds_alternative<Null>(left) || std::holds_alternative<Null>(right)) {
        return false;
    }

    // 如果类型相同，则比较值
    return std::visit([&](const auto & lv) -> bool {
        return std::visit([&](const auto & rv) -> bool {
            using LT = std::decay_t<decltype(lv)>;
            using RT = std::decay_t<decltype(rv)>;

            // 如果类型相同，直接比较
            if constexpr (std::is_same_v<LT, RT>) {
                return lv == rv;
            }

            // 如果有 Decimal 类型，与基本数值类型比较
            if constexpr (std::is_same_v<LT, Decimal>) {
                if constexpr (std::is_integral_v<RT> || std::is_floating_point_v<RT>) {
                    // Decimal 转换为 double 与基本类型比较
                    return lv.to_double() == static_cast<double>(rv);
                } else {
                    return false;
                }
            } else if constexpr (std::is_same_v<RT, Decimal>) {
                if constexpr (std::is_integral_v<LT> || std::is_floating_point_v<LT>) {
                    // 基本类型转换为 double 与 Decimal 比较
                    return static_cast<double>(lv) == rv.to_double();
                } else {
                    return false;
                }
            }

            // 如果都是基本数值类型（整数或浮点数），转换为 double 比较
            else if constexpr ((std::is_integral_v<LT> || std::is_floating_point_v<LT>) && 
                               (std::is_integral_v<RT> || std::is_floating_point_v<RT>)) {
                return to_double_value(lv) == to_double_value(rv);
            }

            // 其他情况不相等
            return false;
        }, right);
    }, left);
}

std::optional<int> Evaluator::compare_values(const FieldValue & left, const FieldValue & right) const
{
    // NULL 值处理：NULL 不能与其他值比较
    if (std::holds_alternative<Null>(left) || std::holds_alternative<Null>(right)) {
        return std::nullopt;
    }

    return std::visit([&](const auto & lv) -> std::optional<int> {
        return std::visit([&](const auto & rv) -> std::optional<int> {
            using LT = std::decay_t<decltype(lv)>;
            using RT = std::decay_t<decltype(rv)>;

            // 如果类型相同，直接比较
            if constexpr (std::is_same_v<LT, RT>) {
                // Null 类型已经在函数开头处理，这里不应该出现
                if constexpr (std::is_same_v<LT, Null>) {
                    return std::nullopt;
                } else {
                    if (lv < rv) return -1;
                    if (lv > rv) return 1;
                    return 0;
                }
            }

            // Decimal 类型需要特殊处理
            if constexpr (std::is_same_v<LT, Decimal>) {
                // Decimal 与整数或浮点数比较
                if constexpr (std::is_integral_v<RT> || std::is_floating_point_v<RT>) {
                    // Decimal 转换为 double 与基本类型比较
                    double l = lv.to_double();
                    double r = static_cast<double>(rv);
                    if (l < r) return -1;
                    if (l > r) return 1;
                    return 0;
                } else {
                    return std::nullopt;
                }
            } else if constexpr (std::is_same_v<RT, Decimal>) {
                // 整数或浮点数与 Decimal 比较
                if constexpr (std::is_integral_v<LT> || std::is_floating_point_v<LT>) {
                    // 基本类型转换为 double 与 Decimal 比较
                    double l = static_cast<double>(lv);
                    double r = rv.to_double();
                    if (l < r) return -1;
                    if (l > r) return 1;
                    return 0;
                } else {
                    return std::nullopt;
                }
            }

            // 字符串类型
            else if constexpr (std::is_same_v<LT, std::string> && std::is_same_v<RT, std::string>) {
                if (lv < rv) return -1;
                if (lv > rv) return 1;
                return 0;
            }

            // 如果都是整数类型，转换为 int64_t 比较
            else if constexpr (std::is_integral_v<LT> && std::is_integral_v<RT>) {
                std::int64_t l = to_int64_value(lv);
                std::int64_t r = to_int64_value(rv);
                if (l < r) return -1;
                if (l > r) return 1;
                return 0;
            }

            // 如果都是浮点数类型，转换为 double 比较
            else if constexpr (std::is_floating_point_v<LT> && std::is_floating_point_v<RT>) {
                double l = to_double_value(lv);
                double r = to_double_value(rv);
                if (l < r) return -1;
                if (l > r) return 1;
                return 0;
            }

            // 如果一个是整数，一个是浮点数，转换为 double 比较
            else if constexpr (std::is_integral_v<LT> && std::is_floating_point_v<RT>) {
                double l = to_double_value(lv);
                double r = to_double_value(rv);
                if (l < r) return -1;
                if (l > r) return 1;
                return 0;
            } else if constexpr (std::is_floating_point_v<LT> && std::is_integral_v<RT>) {
                double l = to_double_value(lv);
                double r = to_double_value(rv);
                if (l < r) return -1;
                if (l > r) return 1;
                return 0;
            }

            // 其他类型不兼容
            else {
                return std::nullopt;
            }
        }, right);
    }, left);
}

} // namespace dreamdb
