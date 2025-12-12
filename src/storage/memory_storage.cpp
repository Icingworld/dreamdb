#include "dreamdb/storage/memory_storage.h"

#include <algorithm>
#include <regex>
#include <stdexcept>

#include "dreamdb/query/query.h"
#include "dreamdb/query/condition.h"
#include "dreamdb/query/order.h"
#include "dreamdb/query/limit.h"
#include "dreamdb/common/null.h"
#include "dreamdb/common/decimal.h"

namespace dreamdb
{

namespace
{

/**
 * @brief 比较两个 FieldValue
 * @return 负数表示 lhs < rhs，0 表示相等，正数表示 lhs > rhs
 */
int compare_field_values(const FieldValue & lhs, const FieldValue & rhs)
{
    return std::visit([](const auto & l, const auto & r) -> int {
        using L = std::decay_t<decltype(l)>;
        using R = std::decay_t<decltype(r)>;

        if constexpr (std::is_same_v<L, R>) {
            if constexpr (std::is_same_v<L, std::int8_t> || std::is_same_v<L, std::int16_t> ||
                          std::is_same_v<L, std::int32_t> || std::is_same_v<L, std::int64_t>) {
                if (l < r) return -1;
                if (l > r) return 1;
                return 0;
            }
            else if constexpr (std::is_same_v<L, float> || std::is_same_v<L, double>) {
                if (l < r) return -1;
                if (l > r) return 1;
                return 0;
            }
            else if constexpr (std::is_same_v<L, std::string>) {
                return l.compare(r);
            }
            else if constexpr (std::is_same_v<L, bool>) {
                if (l == r) return 0;
                return l ? 1 : -1;
            }
            else if constexpr (std::is_same_v<L, Decimal>) {
                if (l < r) return -1;
                if (l > r) return 1;
                return 0;
            }
            else if constexpr (std::is_same_v<L, std::vector<float>>) {
                // 向量比较：先比较长度，再比较元素
                if (l.size() != r.size()) {
                    return l.size() < r.size() ? -1 : 1;
                }
                for (std::size_t i = 0; i < l.size(); ++i) {
                    if (l[i] < r[i]) return -1;
                    if (l[i] > r[i]) return 1;
                }
                return 0;
            }
            else if constexpr (std::is_same_v<L, Null>) {
                return 0;  // Null 总是相等
            }
        }

        // 类型不匹配，按类型名称排序（用于错误处理）
        return 0;
    }, lhs, rhs);
}

/**
 * @brief LIKE 模式匹配
 * @param text 要匹配的文本
 * @param pattern LIKE 模式（% 匹配任意字符，_ 匹配单个字符）
 * @return 是否匹配
 */
bool match_like_pattern(const std::string & text, const std::string & pattern)
{
    // 将 SQL LIKE 模式转换为正则表达式
    std::string regex_pattern;
    regex_pattern.reserve(pattern.size() * 2);

    for (char c : pattern) {
        if (c == '%') {
            regex_pattern += ".*";
        }
        else if (c == '_') {
            regex_pattern += ".";
        }
        else if (c == '.' || c == '^' || c == '$' || c == '|' || c == '(' || c == ')' ||
                 c == '[' || c == ']' || c == '{' || c == '}' || c == '+' || c == '*' || c == '?') {
            // 转义正则表达式特殊字符
            regex_pattern += '\\';
            regex_pattern += c;
        }
        else {
            regex_pattern += c;
        }
    }

    try {
        std::regex regex(regex_pattern, std::regex_constants::icase);
        return std::regex_match(text, regex);
    } catch (const std::regex_error &) {
        // 正则表达式错误，返回 false
        return false;
    }
}

/**
 * @brief 评估单个条件是否匹配实体
 * @param entity 实体
 * @param condition 条件
 * @return 是否匹配
 */
bool evaluate_condition(const Entity & entity, const Condition & condition)
{
    // 组合条件（AND, OR）
    if (condition.is_composite()) {
        // 获取组合条件类型
        const auto logic_op = condition.get_logic_operator();
        // 获取左条件
        const auto * left = condition.get_left();
        // 获取右条件
        const auto * right = condition.get_right();

        // 如果左条件或右条件为空，则不匹配
        if (!left || !right) {
            return false;
        }

        // 递归评估左条件和右条件
        bool left_result = evaluate_condition(entity, *left);
        bool right_result = evaluate_condition(entity, *right);

        if (logic_op == LogicOperator::AND) {
            return left_result && right_result;
        }
        else if (logic_op == LogicOperator::OR) {
            return left_result || right_result;
        }

        // 理论上不会执行到这里
        return false;
    }

    // 字段索引是通用成员，取出判断是否有效，并取出值

    // 获取字段索引
    const auto field_index_opt = condition.get_field_index();
    if (!field_index_opt) {
        return false;
    }

    const std::size_t field_index = *field_index_opt;

    // 检查字段索引是否有效
    if (field_index >= entity.field_count()) {
        return false;
    }

    const FieldValue & field_value = entity.get_value(field_index);

    // BETWEEN 条件
    if (condition.is_between()) {
        const auto min_opt = condition.get_min_value();
        const auto max_opt = condition.get_max_value();
        if (!min_opt || !max_opt) {
            return false;
        }

        int cmp_min = compare_field_values(field_value, *min_opt);
        int cmp_max = compare_field_values(field_value, *max_opt);
        return cmp_min >= 0 && cmp_max <= 0;
    }

    // IN 条件
    if (condition.is_in()) {
        const auto values = condition.get_values();
        for (const auto & value : values) {
            if (compare_field_values(field_value, value) == 0) {
                return true;
            }
        }
        return false;
    }

    // LIKE 条件
    if (condition.is_like()) {
        const auto pattern_opt = condition.get_pattern();
        if (!pattern_opt) {
            return false;
        }

        // LIKE 只支持字符串类型
        if (!std::holds_alternative<std::string>(field_value)) {
            return false;
        }

        const std::string & text = std::get<std::string>(field_value);
        return match_like_pattern(text, *pattern_opt);
    }

    // 单个值条件（EQ, NE, GT, GE, LT, LE）
    const auto value_opt = condition.get_value();
    if (!value_opt) {
        return false;
    }

    const ConditionType type = condition.get_condition_type();
    int cmp = compare_field_values(field_value, *value_opt);

    switch (type) {
        case ConditionType::EQ:
            return cmp == 0;
        case ConditionType::NE:
            return cmp != 0;
        case ConditionType::GT:
            return cmp > 0;
        case ConditionType::GE:
            return cmp >= 0;
        case ConditionType::LT:
            return cmp < 0;
        case ConditionType::LE:
            return cmp <= 0;
        default:
            return false;
    }
}

/**
 * @brief 比较两个实体（用于排序）
 * @param lhs 左实体
 * @param rhs 右实体
 * @param order 排序规则
 * @return true 表示 lhs 应该在 rhs 之前
 */
bool compare_entities(const Entity & lhs, const Entity & rhs, const Order & order)
{
    const std::size_t field_index = order.get_field_index();
    
    if (field_index >= lhs.field_count() || field_index >= rhs.field_count()) {
        return false;
    }

    const FieldValue & lhs_value = lhs.get_value(field_index);
    const FieldValue & rhs_value = rhs.get_value(field_index);

    int cmp = compare_field_values(lhs_value, rhs_value);

    if (order.get_direction() == Direction::ASC) {
        return cmp < 0;
    }
    else {
        return cmp > 0;
    }
}

} // namespace

MemoryStorage::MemoryStorage() noexcept
    : entity_map_{}
{
}

MutationResult MemoryStorage::insert(const Entity & entity)
{
    // 查找是否有该 id 的实体
    auto it = entity_map_.find(entity.get_id());
    if (it != entity_map_.end()) {
        // 已经存在了，失败
        // 理论上不应该出现该情况
        return MutationResult::make_failure("Entity id already exists");
    }

    // 插入实体
    entity_map_[entity.get_id()] = entity;
    return MutationResult::make_success(1);
}

MutationResult MemoryStorage::remove_by_id(std::int64_t id)
{
    // 查找是否有该 id 的实体
    auto it = entity_map_.find(id);
    if (it == entity_map_.end()) {
        return MutationResult::make_failure("Entity id not found");
    }

    entity_map_.erase(it);
    return MutationResult::make_success(1);
}

MutationResult MemoryStorage::update_by_id(std::int64_t id, std::vector<std::pair<std::size_t, FieldValue>> fields)
{
    auto it = entity_map_.find(id);
    if (it == entity_map_.end()) {
        return MutationResult::make_failure("Entity id not found");
    }

    for (const auto & [index, value] : fields) {
        it->second.set_value(index, value);
    }

    return MutationResult::make_success(1);
}

std::unique_ptr<Entity> MemoryStorage::get_by_id(std::int64_t id) const
{
    auto it = entity_map_.find(id);
    if (it == entity_map_.end()) {
        return nullptr;
    }

    // 构造一个新的实体返回
    return std::make_unique<Entity>(it->second);
}

std::vector<std::unique_ptr<Entity>> MemoryStorage::query(const Query & query) const
{
    std::vector<std::unique_ptr<Entity>> results;

    // 1. 条件过滤（WHERE）
    if (query.has_condition()) {
        const Condition & condition = *query.get_condition();
        for (const auto & [id, entity] : entity_map_) {
            // 评估每条记录是否满足条件
            if (evaluate_condition(entity, condition)) {
                results.push_back(std::make_unique<Entity>(entity));
            }
        }
    }
    else {
        // 没有条件，返回所有实体
        results.reserve(entity_map_.size());
        for (const auto & [id, entity] : entity_map_) {
            results.push_back(std::make_unique<Entity>(entity));
        }
    }

    // 2. 排序（ORDER BY）
    if (query.has_order()) {
        const Order & order = *query.get_order();
        std::sort(results.begin(), results.end(),
                  [&order](const std::unique_ptr<Entity> & lhs, const std::unique_ptr<Entity> & rhs) {
                      return compare_entities(*lhs, *rhs, order);
                  });
    }

    // 3. 限制（LIMIT）
    if (query.has_limit()) {
        const Limit & limit = *query.get_limit();
        const std::int64_t limit_value = limit.get_limit();
        if (limit_value >= 0 && static_cast<std::size_t>(limit_value) < results.size()) {
            results.resize(static_cast<std::size_t>(limit_value));
        }
    }

    return results;
}

std::size_t MemoryStorage::size() const
{
    return entity_map_.size();
}

bool MemoryStorage::empty() const
{
    return entity_map_.empty();
}

bool MemoryStorage::contains(std::int64_t id) const
{
    return entity_map_.find(id) != entity_map_.end();
}

MutationResult MemoryStorage::clear()
{
    entity_map_.clear();
    return MutationResult::make_success(1);
}

} // namespace dreamdb
