#include "dreamdb/query/limit.h"

namespace dreamdb
{

Limit::Limit(std::int64_t limit) noexcept
    : limit_(limit)
{
}

std::int64_t Limit::get_limit() const noexcept
{
    return limit_;
}

} // namespace dreamdb
