#pragma once

#include <optional>

#include "dreamdb/parser/ast/ast_expression_node.h"
#include "dreamdb/schema/field.h"
#include "dreamdb/evaluator/evaluator_context.h"

namespace dreamdb
{

/**
 * @brief 评估结果
 */
class EvaluateResult
{
public:
    explicit EvaluateResult() noexcept;

    EvaluateResult(const EvaluateResult &) = default;

    EvaluateResult(EvaluateResult &&) noexcept = default;

    EvaluateResult & operator=(const EvaluateResult &) = default;

    EvaluateResult & operator=(EvaluateResult &&) noexcept = default;

    ~EvaluateResult() = default;

public:
    /**
     * @brief 创建成功结果
     * @param value 评估结果值
     * @return 成功的结果对象
     */
    static EvaluateResult make_success(const FieldValue & value);
    
    /**
     * @brief 创建失败结果
     * @param error_message 错误信息
     * @return 失败的结果对象
     */
    static EvaluateResult make_error(const std::string & error_message);

public:
    /**
     * @brief 设置评估结果值
     * @param value 评估结果值
     */
    void set_value(const FieldValue & value);
    
    /**
     * @brief 设置是否成功
     * @param is_success 是否成功
     */
    void set_is_success(bool is_success) noexcept;
    
    /**
     * @brief 设置错误信息
     * @param error_message 错误信息
     */
    void set_error_message(const std::string & error_message);

    /**
     * @brief 获取评估结果值
     * @return 评估结果值
     */
    const FieldValue & get_value() const noexcept;

    /**
     * @brief 获取是否成功
     * @return 是否成功
     */
    bool get_is_success() const noexcept;

    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    const std::string & get_error_message() const noexcept;

private:
    FieldValue value_;                       // 评估结果值
    bool is_success_;                        // 是否成功
    std::string error_message_;              // 错误信息
};

/**
 * @brief 表达式评估器
 * @details 将 AST 表达式节点评估为 FieldValue
 * 
 * 支持的表达式类型：
 * - LITERAL_EXPR：字面量（整数、浮点数、字符串等）
 * - IDENTIFIER_EXPR：字段引用（从 Entity 中读取值）
 * - BINARY_EXPR：二元表达式（算术、比较、逻辑运算）
 * - UNARY_EXPR：一元表达式（负号、逻辑非等）
 * - LIKE_EXPR：LIKE 模式匹配
 * - IN_EXPR：IN 列表匹配
 * - BETWEEN_EXPR：BETWEEN 范围匹配
 * - NULL_EXPR：NULL 检查
 * - FUNCTION_CALL_EXPR：函数调用（暂不支持）
 */
class Evaluator
{
public:
    explicit Evaluator() noexcept = default;

    Evaluator(const Evaluator &) = delete;

    Evaluator(Evaluator &&) noexcept = default;

    Evaluator & operator=(const Evaluator &) = delete;

    Evaluator & operator=(Evaluator &&) noexcept = default;

    ~Evaluator() = default;

public:
    /**
     * @brief 评估表达式
     * @param expr 表达式
     * @param context 评估上下文
     * @param field_type 字段类型（可选）
     * @return 表达式评估结果 
     */
    EvaluateResult evaluate(
        const AstExpressionNode * expr,
        const EvaluatorContext & context,
        std::optional<FieldType> field_type = std::nullopt
    ) const;

    /**
     * @brief 评估条件表达式
     * @param expr 条件表达式
     * @param context 评估上下文
     * @return 条件表达式评估结果
     */
    std::optional<bool> evaluate_condition(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

private:
    /**
     * @brief 评估字面量表达式
     * @param expr 字面量表达式
     * @param target_type 目标字段类型
     * @return 字面量表达式评估结果
     */
    EvaluateResult evaluate_literal(
        const AstExpressionNode * expr,
        std::optional<FieldType> target_type
    ) const;

    /**
     * @brief 评估字段引用表达式
     * @param expr 字段引用表达式
     * @param context 评估上下文
     * @return 字段引用表达式评估结果
     */
    EvaluateResult evaluate_identifier(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 评估二元表达式
     * @param expr 二元表达式
     * @param context 评估上下文
     * @return 二元表达式评估结果
     */
    EvaluateResult evaluate_binary(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 评估一元表达式
     * @param expr 一元表达式
     * @param context 评估上下文
     * @return 一元表达式评估结果
     */
    EvaluateResult evaluate_unary(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 评估 LIKE 表达式
     * @param expr LIKE 表达式
     * @param context 评估上下文
     * @return LIKE 表达式评估结果
     */
    EvaluateResult evaluate_like(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 评估 IN 表达式
     * @param expr IN 表达式
     * @param context 评估上下文
     * @return IN 表达式评估结果
     */
    EvaluateResult evaluate_in(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 评估 BETWEEN 表达式
     * @param expr BETWEEN 表达式
     * @param context 评估上下文
     * @return BETWEEN 表达式评估结果
     */
    EvaluateResult evaluate_between(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 评估 NULL 表达式
     * @param expr NULL 表达式
     * @param context 评估上下文
     * @return NULL 表达式评估结果
     */
    EvaluateResult evaluate_null(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 评估函数调用表达式
     * @param expr 函数调用表达式
     * @param context 评估上下文
     * @return 函数调用表达式评估结果
     */
    EvaluateResult evaluate_function_call(
        const AstExpressionNode * expr,
        const EvaluatorContext & context
    ) const;

    /**
     * @brief 将字段值转换为布尔值
     * @param value 字段值
     * @return 布尔值
     */
    bool field_value_to_bool(const FieldValue & value) const;

    /**
     * @brief 比较两个字段值是否相等
     * @param left_value 左字段值
     * @param right_value 右字段值
     * @return 是否相等
     */
    bool compare_values_equal(const FieldValue & left_value, const FieldValue & right_value) const;

    /**
     * @brief 比较两个 FieldValue 的大小关系
     * @param left 左值
     * @param right 右值
     * @return 比较结果：-1 (left < right), 0 (left == right), 1 (left > right)
     *         如果类型不兼容返回 std::nullopt
     */
    std::optional<int> compare_values(const FieldValue & left, const FieldValue & right) const;
};

} // namespace dreamdb
