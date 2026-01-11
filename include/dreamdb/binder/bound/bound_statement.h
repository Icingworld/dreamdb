#pragma once

#include <cstdint>

namespace dreamdb
{

enum class BoundStatementType : std::uint8_t
{
    BINDER_BOUND_SELECT_STATEMENT,      // 绑定后的选择语句
    BINDER_BOUND_INSERT_STATEMENT,      // 绑定后的插入语句
    BINDER_BOUND_UPDATE_STATEMENT,      // 绑定后的更新语句
    BINDER_BOUND_DELETE_STATEMENT       // 绑定后的删除语句
};

/**
 * @brief 绑定后的语句
 */
class BoundStatement
{
protected:
    explicit BoundStatement(BoundStatementType type) noexcept;

public:
    virtual ~BoundStatement() noexcept = default;

public:
    BoundStatementType get_type() const noexcept;

private:
    BoundStatementType type_;          // 语句类型
};

} // namespace dreamdb
