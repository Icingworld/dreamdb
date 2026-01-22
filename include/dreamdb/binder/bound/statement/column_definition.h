#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "dreamdb/common/logical_type.h"
#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 列修饰符
 */
enum class BoundColumnModifier : std::uint8_t
{
    NotNull,            // NOT NULL
    Unique,             // UNIQUE
    PrimaryKey,         // PRIMARY KEY
    AutoIncrement,      // AUTO_INCREMENT
    Default             // DEFAULT
};

/**
 * @brief 绑定列定义
 */
struct BoundColumnDefinition
{
    std::string name;                               // 列名
    common::LogicalType type;                       // 列类型
    std::vector<BoundColumnModifier> modifiers;     // 列修饰符列表
    std::unique_ptr<BoundExpression> default_value; // 默认值表达式
};

} // namespace dreamdb::binder::bound
