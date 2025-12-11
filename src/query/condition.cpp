#include "dreamdb/query/condition.h"

#include <stdexcept>

namespace dreamdb
{

Condition::Condition(std::uint8_t field_index, ConditionType condition_type, const FieldValue & value)
    : kind_(ConditionKind::SINGLE)
    , field_index_(field_index)
    , condition_type_(condition_type)
    , value_(value)
    , logic_op_(LogicOperator::AND)
{
    // 验证条件类型
    if (condition_type == ConditionType::BETWEEN || 
        condition_type == ConditionType::IN || 
        condition_type == ConditionType::LIKE) {
        throw std::invalid_argument("Use specialized constructor for BETWEEN, IN, or LIKE conditions");
    }
}

Condition::Condition(std::uint8_t field_index, const FieldValue & min_value, const FieldValue & max_value)
    : kind_(ConditionKind::BETWEEN)
    , field_index_(field_index)
    , condition_type_(ConditionType::BETWEEN)
    , min_value_(min_value)
    , max_value_(max_value)
    , logic_op_(LogicOperator::AND)
{
}

Condition::Condition(std::uint8_t field_index, const std::vector<FieldValue> & values)
    : kind_(ConditionKind::IN)
    , field_index_(field_index)
    , condition_type_(ConditionType::IN)
    , values_(values)
    , logic_op_(LogicOperator::AND)
{
    if (values.empty()) {
        throw std::invalid_argument("IN condition requires at least one value");
    }
}

Condition::Condition(std::uint8_t field_index, const std::string & pattern)
    : kind_(ConditionKind::LIKE)
    , field_index_(field_index)
    , condition_type_(ConditionType::LIKE)
    , pattern_(pattern)
    , logic_op_(LogicOperator::AND)
{
}

Condition::Condition(LogicOperator logic_op, const Condition & left, const Condition & right)
    : kind_(ConditionKind::COMPOSITE)
    , field_index_(0)
    , condition_type_(ConditionType::EQ)  // 占位符
    , logic_op_(logic_op)
    , left_(std::make_unique<Condition>(left))
    , right_(std::make_unique<Condition>(right))
{
}

Condition::Condition(const Condition & other)
    : kind_(other.kind_)
    , field_index_(other.field_index_)
    , condition_type_(other.condition_type_)
    , value_(other.value_)
    , min_value_(other.min_value_)
    , max_value_(other.max_value_)
    , values_(other.values_)
    , pattern_(other.pattern_)
    , logic_op_(other.logic_op_)
{
    if (other.left_) {
        left_ = std::make_unique<Condition>(*other.left_);
    }
    if (other.right_) {
        right_ = std::make_unique<Condition>(*other.right_);
    }
}

Condition::Condition(Condition && other) noexcept
    : kind_(other.kind_)
    , field_index_(other.field_index_)
    , condition_type_(other.condition_type_)
    , value_(std::move(other.value_))
    , min_value_(std::move(other.min_value_))
    , max_value_(std::move(other.max_value_))
    , values_(std::move(other.values_))
    , pattern_(std::move(other.pattern_))
    , logic_op_(other.logic_op_)
    , left_(std::move(other.left_))
    , right_(std::move(other.right_))
{
}

Condition & Condition::operator=(const Condition & other)
{
    if (this != &other) {
        kind_ = other.kind_;
        field_index_ = other.field_index_;
        condition_type_ = other.condition_type_;
        value_ = other.value_;
        min_value_ = other.min_value_;
        max_value_ = other.max_value_;
        values_ = other.values_;
        pattern_ = other.pattern_;
        logic_op_ = other.logic_op_;

        if (other.left_) {
            left_ = std::make_unique<Condition>(*other.left_);
        } else {
            left_.reset();
        }

        if (other.right_) {
            right_ = std::make_unique<Condition>(*other.right_);
        } else {
            right_.reset();
        }
    }
    return *this;
}

Condition & Condition::operator=(Condition && other) noexcept
{
    if (this != &other) {
        kind_ = other.kind_;
        field_index_ = other.field_index_;
        condition_type_ = other.condition_type_;
        value_ = std::move(other.value_);
        min_value_ = std::move(other.min_value_);
        max_value_ = std::move(other.max_value_);
        values_ = std::move(other.values_);
        pattern_ = std::move(other.pattern_);
        logic_op_ = other.logic_op_;
        left_ = std::move(other.left_);
        right_ = std::move(other.right_);
    }
    return *this;
}

ConditionType Condition::get_condition_type() const noexcept
{
    if (kind_ == ConditionKind::COMPOSITE) {
        return ConditionType::EQ;  // 占位符
    }
    return condition_type_;
}

std::optional<LogicOperator> Condition::get_logic_operator() const noexcept
{
    if (kind_ == ConditionKind::COMPOSITE) {
        return logic_op_;
    }
    return std::nullopt;
}

std::optional<std::uint8_t> Condition::get_field_index() const noexcept
{
    if (kind_ == ConditionKind::COMPOSITE) {
        return std::nullopt;
    }
    return field_index_;
}

std::optional<FieldValue> Condition::get_value() const
{
    if (kind_ == ConditionKind::SINGLE) {
        return value_;
    }
    return std::nullopt;
}

std::optional<FieldValue> Condition::get_min_value() const
{
    if (kind_ == ConditionKind::BETWEEN) {
        return min_value_;
    }
    return std::nullopt;
}

std::optional<FieldValue> Condition::get_max_value() const
{
    if (kind_ == ConditionKind::BETWEEN) {
        return max_value_;
    }
    return std::nullopt;
}

std::vector<FieldValue> Condition::get_values() const
{
    if (kind_ == ConditionKind::IN) {
        return values_;
    }
    return {};
}

const Condition * Condition::get_left() const noexcept
{
    if (kind_ == ConditionKind::COMPOSITE) {
        return left_.get();
    }
    return nullptr;
}

const Condition * Condition::get_right() const noexcept
{
    if (kind_ == ConditionKind::COMPOSITE) {
        return right_.get();
    }
    return nullptr;
}

bool Condition::is_composite() const noexcept
{
    return kind_ == ConditionKind::COMPOSITE;
}

bool Condition::is_between() const noexcept
{
    return kind_ == ConditionKind::BETWEEN;
}

bool Condition::is_in() const noexcept
{
    return kind_ == ConditionKind::IN;
}

bool Condition::is_like() const noexcept
{
    return kind_ == ConditionKind::LIKE;
}

std::optional<std::string> Condition::get_pattern() const
{
    if (kind_ == ConditionKind::LIKE) {
        return pattern_;
    }
    return std::nullopt;
}

Condition Condition::operator&&(const Condition & other) const
{
    return Condition(LogicOperator::AND, *this, other);
}

Condition Condition::operator||(const Condition & other) const
{
    return Condition(LogicOperator::OR, *this, other);
}

} // namespace dreamdb

