#pragma once

#include "dreamdb/binder/bound/statement/statement.h"
#include "dreamdb/common/ids.h"

namespace dreamdb::binder::bound
{

class BoundStatementVisitor;

/**
 * @brief 绑定后的 USE 语句
 */
class BoundUseStatement final : public BoundStatement
{
public:
    explicit BoundUseStatement(dreamdb::common::database_id_t database_id) noexcept;

    ~BoundUseStatement() noexcept override = default;

public:
    /**
     * @brief 接受语句访问者
     * @param visitor 语句访问者
     */
    void accept(BoundStatementVisitor & visitor) const override;

public:
    /**
     * @brief 获取数据库 ID
     * @return 数据库 ID
     */
    dreamdb::common::database_id_t database_id() const noexcept;

private:
    dreamdb::common::database_id_t database_id_;  // 数据库 ID
};

} // namespace dreamdb::binder::bound
