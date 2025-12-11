#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>

#include "dreamdb/schema/field.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 查询条件类
 * @details 用于表示 WHERE 子句中的查询条件，支持等值、范围、IN、BETWEEN、LIKE 等操作
 */
class Condition
{
public:
    Condition() = delete;

    /**
     * @brief 构造函数：创建单个值条件（EQ, NE, GT, GE, LT, LE）
     * @param field_index 字段索引
     * @param condition_type 条件类型
     * @param value 字段值
     */
    Condition(std::uint8_t field_index, ConditionType condition_type, const FieldValue & value);

    /**
     * @brief 构造函数：创建 BETWEEN 条件
     * @param field_index 字段索引
     * @param min_value 最小值
     * @param max_value 最大值
     */
    Condition(std::uint8_t field_index, const FieldValue & min_value, const FieldValue & max_value);

    /**
     * @brief 构造函数：创建 IN 条件
     * @param field_index 字段索引
     * @param values 值列表
     */
    Condition(std::uint8_t field_index, const std::vector<FieldValue> & values);

    /**
     * @brief 构造函数：创建 LIKE 条件
     * @param field_index 字段索引
     * @param pattern 匹配模式（支持 % 和 _ 通配符）
     */
    Condition(std::uint8_t field_index, const std::string & pattern);

    /**
     * @brief 构造函数：创建逻辑组合条件（AND, OR）
     * @param logic_op 逻辑操作符
     * @param left 左条件
     * @param right 右条件
     */
    Condition(LogicOperator logic_op, const Condition & left, const Condition & right);

    /**
     * @brief 拷贝构造函数
     */
    Condition(const Condition & other);

    /**
     * @brief 移动构造函数
     */
    Condition(Condition && other) noexcept;

    /**
     * @brief 拷贝赋值运算符
     */
    Condition & operator=(const Condition & other);

    /**
     * @brief 移动赋值运算符
     */
    Condition & operator=(Condition && other) noexcept;

    ~Condition() = default;

public:
    /**
     * @brief 获取条件类型
     * @return 条件类型
     */
    ConditionType get_condition_type() const noexcept;

    /**
     * @brief 获取逻辑操作符（如果是组合条件）
     * @return 逻辑操作符，如果不是组合条件返回 std::nullopt
     */
    std::optional<LogicOperator> get_logic_operator() const noexcept;

    /**
     * @brief 获取字段索引
     * @return 字段索引，如果是组合条件返回 std::nullopt
     */
    std::optional<std::uint8_t> get_field_index() const noexcept;

    /**
     * @brief 获取单个值（用于 EQ, NE, GT, GE, LT, LE）
     * @return 字段值，如果不是单个值条件返回 std::nullopt
     */
    std::optional<FieldValue> get_value() const;

    /**
     * @brief 获取最小值（用于 BETWEEN）
     * @return 最小值，如果不是 BETWEEN 条件返回 std::nullopt
     */
    std::optional<FieldValue> get_min_value() const;

    /**
     * @brief 获取最大值（用于 BETWEEN）
     * @return 最大值，如果不是 BETWEEN 条件返回 std::nullopt
     */
    std::optional<FieldValue> get_max_value() const;

    /**
     * @brief 获取值列表（用于 IN）
     * @return 值列表，如果不是 IN 条件返回空列表
     */
    std::vector<FieldValue> get_values() const;

    /**
     * @brief 获取左条件（用于组合条件）
     * @return 左条件指针，如果不是组合条件返回 nullptr
     */
    const Condition * get_left() const noexcept;

    /**
     * @brief 获取右条件（用于组合条件）
     * @return 右条件指针，如果不是组合条件返回 nullptr
     */
    const Condition * get_right() const noexcept;

    /**
     * @brief 检查是否为组合条件
     * @return 如果是组合条件返回 true
     */
    bool is_composite() const noexcept;

    /**
     * @brief 检查是否为 BETWEEN 条件
     * @return 如果是 BETWEEN 条件返回 true
     */
    bool is_between() const noexcept;

    /**
     * @brief 检查是否为 IN 条件
     * @return 如果是 IN 条件返回 true
     */
    bool is_in() const noexcept;

    /**
     * @brief 检查是否为 LIKE 条件
     * @return 如果是 LIKE 条件返回 true
     */
    bool is_like() const noexcept;

    /**
     * @brief 获取 LIKE 模式（用于 LIKE 条件）
     * @return 匹配模式，如果不是 LIKE 条件返回 std::nullopt
     */
    std::optional<std::string> get_pattern() const;

public:
    /**
     * @brief 创建 AND 组合条件
     * @param other 另一个条件
     * @return 组合后的条件
     */
    Condition operator&&(const Condition & other) const;

    /**
     * @brief 创建 OR 组合条件
     * @param other 另一个条件
     * @return 组合后的条件
     */
    Condition operator||(const Condition & other) const;

private:
    ConditionKind kind_;                    // 条件类型

    // 单个值条件
    std::uint8_t field_index_;              // 字段索引
    ConditionType condition_type_;          // 条件类型
    FieldValue value_;                      // 字段值

    // BETWEEN 条件
    FieldValue min_value_;                  // 最小值
    FieldValue max_value_;                  // 最大值

    // IN 条件
    std::vector<FieldValue> values_;        // 值列表

    // LIKE 条件
    std::string pattern_;                   // 匹配模式

    // 组合条件
    LogicOperator logic_op_;                // 逻辑操作符
    std::unique_ptr<Condition> left_;       // 左条件
    std::unique_ptr<Condition> right_;      // 右条件
};

} // namespace dreamdb
