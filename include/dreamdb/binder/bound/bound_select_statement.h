#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "dreamdb/expression/expression.h"
#include "dreamdb/binder/bound/bound_statement.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

struct BoundSelectItem
{
    std::unique_ptr<Expression> expr;
    std::string alias;                  // 输出别名，允许为空
};

struct BoundOrderByItem
{
    std::unique_ptr<Expression> expr;
    Direction order;
};

class BoundSelectStatement : public BoundStatement
{
public:
    explicit BoundSelectStatement();

    ~BoundSelectStatement() noexcept override = default;

public:
    std::vector<BoundSelectItem> select_items;    // 选择项
    std::size_t collection_id;                    // 集合 ID
    std::unique_ptr<Expression> where;            // 条件表达式
    std::vector<std::unique_ptr<Expression>> group_by;       // 分组表达式
    std::unique_ptr<Expression> having;           // 过滤表达式
    std::vector<BoundOrderByItem> order_by;       // 排序表达式
    std::optional<std::size_t> limit;             // 限制
    std::optional<std::size_t> offset;            // 偏移
    // 这里使用了很多 std::size_t 类型，因为目前不需要支持表达的能力，限定其为常量即可
    // 后续添加参数功能时，可改为 std::unique_ptr<Expression>
};

} // namespace dreamdb
