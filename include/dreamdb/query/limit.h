#pragma once

#include <cstdint>

namespace dreamdb
{

/**
 * @brief 限制类
 */
class Limit
{
public:
    Limit() = delete;

    explicit Limit(std::int64_t limit) noexcept;

    Limit(const Limit & other) = default;

    Limit(Limit && other) noexcept = default;

    Limit & operator=(const Limit & other) = default;

    Limit & operator=(Limit && other) noexcept = default;

    ~Limit() = default;

public:
    /**
     * @brief 获取限制数量
     * @return 限制数量
     */
    std::int64_t get_limit() const noexcept;

private:
    std::int64_t limit_;        // 限制数量
};

} // namespace dreamdb
