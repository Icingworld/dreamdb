#include "dreamdb/index/index_key.h"
#include "dreamdb/common/decimal.h"
#include "dreamdb/common/null.h"
#include <algorithm>
#include <functional>

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
 * @brief 计算单个 FieldValue 的哈希值
 */
std::size_t hash_field_value(const FieldValue & value)
{
    return std::visit([](const auto & v) -> std::size_t {
        using T = std::decay_t<decltype(v)>;
        
        if constexpr (std::is_same_v<T, std::int8_t> || 
                      std::is_same_v<T, std::int16_t> || 
                      std::is_same_v<T, std::int32_t> || 
                      std::is_same_v<T, std::int64_t>) {
            return std::hash<T>{}(v);
        }
        else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return std::hash<T>{}(v);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return std::hash<std::string>{}(v);
        }
        else if constexpr (std::is_same_v<T, bool>) {
            return std::hash<bool>{}(v);
        }
        else if constexpr (std::is_same_v<T, Decimal>) {
            // Decimal 哈希：使用其内部值
            return std::hash<std::int64_t>{}(v.value());
        }
        else if constexpr (std::is_same_v<T, std::vector<float>>) {
            // 向量哈希：组合所有元素的哈希
            std::size_t hash = 0;
            for (float f : v) {
                hash ^= std::hash<float>{}(f) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
        else if constexpr (std::is_same_v<T, Null>) {
            return 0;  // Null 的哈希值固定为 0
        }
        else {
            return 0;
        }
    }, value);
}

} // namespace

IndexKey::IndexKey(const std::vector<FieldValue> & values)
    : values_(values)
{
}

const std::vector<FieldValue> & IndexKey::get_values() const noexcept
{
    return values_;
}

int IndexKey::compare(const ComparableIndexKeyBase& other) const
{
    // 将 other 转换为 IndexKey（需要类型检查）
    const IndexKey* other_key = dynamic_cast<const IndexKey*>(&other);
    if (other_key == nullptr) {
        // 如果不是 IndexKey 类型，无法比较
        return 0;  // 或者可以抛出异常，这里返回 0 表示相等
    }
    
    const std::vector<FieldValue>& other_values = other_key->values_;
    std::size_t min_size = std::min(values_.size(), other_values.size());
    
    // 字典序比较
    for (std::size_t i = 0; i < min_size; ++i) {
        int cmp = compare_field_values(values_[i], other_values[i]);
        if (cmp != 0) {
            return cmp;
        }
    }
    
    // 如果前缀都相同，比较长度
    if (values_.size() < other_values.size()) return -1;
    if (values_.size() > other_values.size()) return 1;
    return 0;
}

std::size_t IndexKey::hash() const
{
    std::size_t hash_value = 0;
    
    // 组合所有字段值的哈希值（使用 boost::hash_combine 算法）
    for (const FieldValue& value : values_) {
        std::size_t value_hash = hash_field_value(value);
        // 组合哈希算法
        hash_value ^= value_hash + 0x9e3779b9 + (hash_value << 6) + (hash_value >> 2);
    }
    
    return hash_value;
}

bool IndexKey::equals(const HashableIndexKeyBase& other) const
{
    const IndexKey* other_key = dynamic_cast<const IndexKey*>(&other);
    if (other_key == nullptr) {
        return false;
    }
    
    // 比较字段值列表
    if (values_.size() != other_key->values_.size()) {
        return false;
    }
    
    for (std::size_t i = 0; i < values_.size(); ++i) {
        if (compare_field_values(values_[i], other_key->values_[i]) != 0) {
            return false;
        }
    }
    
    return true;
}

} // namespace dreamdb
