#pragma once

#include <cstddef>
#include <string>

#include "dreamdb/common/ids.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::catalog
{

struct column_info_t
{
    // 基本信息
    common::column_id_t id;                 // 列 ID
    std::size_t index;                      // 列索引
    std::string name;                       // 列名称

    // 类型
    common::LogicalType logical_type;       // 逻辑类型

    // 可空性
    bool is_nullable;                       // 是否可为空

    // INSERT 相关
    bool is_insertable;                     // 是否可插入，隐藏列、系统列、计算列等不能插入
};

} // namespace dreamdb::catalog
