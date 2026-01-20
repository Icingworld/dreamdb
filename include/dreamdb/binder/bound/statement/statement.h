#pragma once

#include <cstdint>

namespace dreamdb::binder::bound
{

/**
 * @brief 绑定后的语句类型
 */
enum class BoundStatementType : std::uint8_t
{
    Select,      // 绑定后的 SELECT 语句
    Insert,      // 绑定后的 INSERT 语句
    Update,      // 绑定后的 UPDATE 语句
    Delete,      // 绑定后的 DELETE 语句
    Use,         // 绑定后的 USE 语句
    Show,        // 绑定后的 SHOW 语句
    Describe,    // 绑定后的 DESCRIBE 语句
    Drop,        // 绑定后的 DROP 语句
    Create,      // 绑定后的 CREATE 语句
    Alter        // 绑定后的 ALTER 语句
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
    /**
     * @brief 获取语句类型
     * @return 语句类型
     */
    BoundStatementType type() const noexcept;

private:
    BoundStatementType type_;          // 语句类型
};

} // namespace dreamdb::binder::bound
