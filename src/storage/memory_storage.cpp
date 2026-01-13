#include "dreamdb/storage/memory_storage.h"

#include <algorithm>
#include <regex>
#include <stdexcept>

#include "dreamdb/query/order.h"
#include "dreamdb/query/limit.h"
#include "dreamdb/common/null.h"
#include "dreamdb/common/decimal.h"
#include "dreamdb/schema/collection.h"
#include "dreamdb/evaluator/evaluator.h"
#include "dreamdb/evaluator/evaluator_context.h"
#include "dreamdb/parser/ast/ast_node.h"

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
    entity_map_.emplace(entity.get_id(), entity);
    return MutationResult::make_success(1);
}

MutationResult MemoryStorage::remove_by_id(std::size_t id)
{
    // 查找是否有该 id 的实体
    auto it = entity_map_.find(id);
    if (it == entity_map_.end()) {
        return MutationResult::make_failure("Entity id not found");
    }

    entity_map_.erase(it);
    return MutationResult::make_success(1);
}

MutationResult MemoryStorage::update_by_id(std::size_t id, std::vector<std::pair<std::size_t, FieldValue>> fields)
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

std::unique_ptr<Entity> MemoryStorage::get_by_id(std::size_t id) const
{
    auto it = entity_map_.find(id);
    if (it == entity_map_.end()) {
        return nullptr;
    }

    // 构造一个新的实体返回
    return std::make_unique<Entity>(it->second);
}

std::vector<std::unique_ptr<Entity>> MemoryStorage::get_all_entities() const
{
    std::vector<std::unique_ptr<Entity>> results;
    results.reserve(entity_map_.size());
    
    for (const auto & [id, entity] : entity_map_) {
        results.push_back(std::make_unique<Entity>(entity));
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

bool MemoryStorage::contains(std::size_t id) const
{
    return entity_map_.find(id) != entity_map_.end();
}

MutationResult MemoryStorage::clear()
{
    entity_map_.clear();
    return MutationResult::make_success(1);
}

} // namespace dreamdb
