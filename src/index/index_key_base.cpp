#include "dreamdb/index/index_key_base.h"

namespace dreamdb
{

bool IndexKeyBase::operator==(const IndexKeyBase & other) const
{
    return compare(other) == 0;
}

bool IndexKeyBase::operator!=(const IndexKeyBase & other) const
{
    return compare(other) != 0;
}

bool IndexKeyBase::operator<(const IndexKeyBase & other) const
{
    return compare(other) < 0;
}

bool IndexKeyBase::operator>(const IndexKeyBase & other) const
{
    return compare(other) > 0;
}

bool IndexKeyBase::operator<=(const IndexKeyBase & other) const
{
    return compare(other) <= 0;
}

bool IndexKeyBase::operator>=(const IndexKeyBase & other) const
{
    return compare(other) >= 0;
}

} // namespace dreamdb
