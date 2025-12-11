#include "dreamdb/common/null.h"

namespace dreamdb
{

bool Null::operator==(const Null & other) const noexcept
{
    return true;
}

bool Null::operator!=(const Null & other) const noexcept
{
    return false;
}

} // namespace dreamdb
