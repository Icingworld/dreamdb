#include "dreamdb/binder/bound/statement/create.h"

#include <cassert>

#include "dreamdb/binder/bound/statement/visitor.h"

namespace dreamdb::binder::bound
{

BoundCreateStatement::BoundCreateStatement(
    bool if_not_exists,
    BoundCreateOperation create_operation
)
    : BoundStatement(BoundStatementType::Create)
    , if_not_exists_(if_not_exists)
    , create_operation_(std::move(create_operation))
{
    // 根据操作类型进行验证
    std::visit([](auto & operation) {
        using T = std::decay_t<decltype(operation)>;

        if constexpr (std::is_same_v<T, BoundCreateDatabase>) {
            // 数据库名称不能为空
            assert(!operation.database_name.empty());
        } else if constexpr (std::is_same_v<T, BoundCreateCollection>) {
            // 集合名称不能为空
            assert(!operation.collection_name.empty());

            // 列定义不能为空
            assert(!operation.column_definitions.empty());
        } else if constexpr (std::is_same_v<T, BoundCreateIndex>) {
            // 索引名称不能为空
            assert(!operation.index_name.empty());

            // 列 ID 列表不能为空
            assert(!operation.column_ids.empty());
        } else if constexpr (std::is_same_v<T, BoundCreateVIndex>) {
            // 向量索引名称不能为空
            assert(!operation.vindex_name.empty());
        }
    }, create_operation_);
}

void BoundCreateStatement::accept(BoundStatementVisitor & visitor) const
{
    visitor.visit(*this);
}

bool BoundCreateStatement::if_not_exists() const noexcept
{
    return if_not_exists_;
}

const BoundCreateOperation & BoundCreateStatement::create_operation() const noexcept
{
    return create_operation_;
}

} // namespace dreamdb::binder::bound
