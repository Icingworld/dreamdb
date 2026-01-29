#pragma once

#include <cstddef>
#include <string>

#include "dreamdb/common/ids.h"
#include "dreamdb/common/logical_type.h"

namespace dreamdb::schema
{

enum class ColumnRole : std::uint8_t
{
    User,         // 用户列
    System        // 系统列
};

struct ColumnDefinition
{
    // 基本信息
    common::column_oid_t oid;               // 列 OID
    std::string name;                       // 列名称

    // 类型
    common::LogicalType logical_type;       // 逻辑类型

    // 列角色
    ColumnRole role;                        // 列角色
};

} // namespace dreamdb::schema
