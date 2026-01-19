#pragma once

#include <string>

#include "dreamdb/common/ids.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::catalog
{

struct column_info_t
{
    common::column_id_t id;                 // 列 ID
    std::string name;                       // 列名称
    common::LogicalType logical_type;       // 逻辑类型
    bool is_nullable;                       // 是否可为空
};

} // namespace dreamdb::catalog