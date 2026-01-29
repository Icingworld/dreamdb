#pragma once

#include <vector>

#include "dreamdb/common/ids.h"
#include "dreamdb/common/type.h"

namespace dreamdb::storage
{

/**
 * @brief 行数据
 */
struct Row
{
    std::vector<dreamdb::common::FieldValue> values;    // 字段值
};

} // namespace dreamdb::storage
