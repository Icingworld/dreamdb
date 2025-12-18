#include "dreamdb/common/null.h"

namespace dreamdb
{

bool Null::operator==(const Null &) const noexcept
{
    return true;
}

bool Null::operator!=(const Null &) const noexcept
{
    return false;
}

} // namespace dreamdb
