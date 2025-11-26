#include "dreamdb/common/type.h"

namespace dreamdb
{

bool NullType::operator==(const NullType & other) const noexcept
{
    return true;
}

bool NullType::operator!=(const NullType & other) const noexcept
{
    return false;
}

} // namespace dreamdb

