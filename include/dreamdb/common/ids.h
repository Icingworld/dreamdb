#pragma once

#include <cstdint>

namespace dreamdb::common
{

using database_id_t = std::uint64_t;        // 数据库 ID 类型
using collection_id_t = std::uint64_t;      // 集合 ID 类型
using column_id_t = std::uint64_t;          // 列 ID 类型
using record_id_t = std::uint64_t;          // 记录 ID 类型

} // namespace dreamdb::common
