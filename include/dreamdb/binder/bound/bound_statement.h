#pragma once

#include <cstdint>

namespace dreamdb
{

enum class BoundStatementType : std::uint8_t
{
    BINDER_BOUND_SELECT_STATEMENT,      // 绑定后的 SELECT 语句
    BINDER_BOUND_INSERT_STATEMENT,      // 绑定后的 INSERT 语句
    BINDER_BOUND_UPDATE_STATEMENT,      // 绑定后的 UPDATE 语句
    BINDER_BOUND_DELETE_STATEMENT,      // 绑定后的 DELETE 语句
    BINDER_BOUND_USE_STATEMENT,         // 绑定后的 USE 语句
    BINDER_BOUND_SHOW_STATEMENT,        // 绑定后的 SHOW 语句
    BINDER_BOUND_DESCRIBE_STATEMENT,    // 绑定后的 DESCRIBE 语句
    BINDER_BOUND_DROP_STATEMENT,        // 绑定后的 DROP 语句
    BINDER_BOUND_CREATE_STATEMENT       // 绑定后的 CREATE 语句
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
