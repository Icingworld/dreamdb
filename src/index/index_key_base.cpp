#include "dreamdb/index/index_key_base.h"

namespace dreamdb
{

bool ComparableIndexKeyBase::operator==(const ComparableIndexKeyBase & other) const
{
    return compare(other) == 0;
}

bool ComparableIndexKeyBase::operator!=(const ComparableIndexKeyBase & other) const
{
    return compare(other) != 0;
}

bool ComparableIndexKeyBase::operator<(const ComparableIndexKeyBase & other) const
{
    return compare(other) < 0;
}

bool ComparableIndexKeyBase::operator>(const ComparableIndexKeyBase & other) const
{
    return compare(other) > 0;
}

bool ComparableIndexKeyBase::operator<=(const ComparableIndexKeyBase & other) const
{
    return compare(other) <= 0;
}

bool ComparableIndexKeyBase::operator>=(const ComparableIndexKeyBase & other) const
{
    return compare(other) >= 0;
}

bool HashableIndexKeyBase::operator==(const HashableIndexKeyBase & other) const
{
    return equals(other);
}

} // namespace dreamdb
