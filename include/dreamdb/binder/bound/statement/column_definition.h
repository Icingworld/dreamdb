#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "dreamdb/common/type.h"
#include "dreamdb/binder/bound/expression/expression.h"

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定字段类型
 */
struct BoundFieldType
{
    common::FieldType type;                  // 字段类型

    // 元数据，储存字段类型后面()中的内容
    std::optional<int> length;               // 长度
    std::optional<int> precision;            // 精度
    std::optional<int> scale;                // 小数位数
    std::optional<std::vector<std::string>> enum_values; // 枚举值
    std::optional<int> dimension;                        // 向量维度
};

/**
 * @brief 绑定列定义
 */
struct BoundColumnDefinition
{
    std::string name;                               // 列名
    BoundFieldType type;                            // 列类型

    // 限定词收束
    std::optional<bool> not_null;                   // NOT NULL
    std::optional<bool> unique;                     // UNIQUE
    std::optional<bool> primary_key;                // PRIMARY KEY
    std::optional<bool> auto_increment;             // AUTO_INCREMENT
    std::unique_ptr<BoundExpression> default_value; // DEFAULT 默认值表达式
};

} // namespace dreamdb::binder::bound
