#include "dreamdb/schema/database.h"

#include <cassert>

namespace dreamdb::schema
{

Database::Database(dreamdb::common::database_id_t id, std::string name)
    : id_(id), name_(std::move(name))
{
    // 数据库名称不能为空
    assert(!name_.empty());
}

dreamdb::common::database_id_t Database::id() const noexcept
{
    return id_;
}

const std::string & Database::name() const noexcept
{
    return name_;
}

} // namespace dreamdb::schema
