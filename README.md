# dreamdb 接口与数据流简要说明

## 架构分层
- **Storage 层**：定义存储抽象 `StorageBase`，当前实现 `MemoryStorage`。
- **Segment 层**：`Segment` 封装单个段的生命周期、状态与实体 CRUD；`SegmentManager` 负责段的创建、查找、状态管理。
- **Schema 层**：`Collection` 管理同构数据的逻辑集合；`CollectionManager` 管理集合；`Database`/`DatabaseManager` 管理数据库级资源。
- **Query/Parser/Executor**：解析 SQL/DSL，生成查询计划并执行，调用上层 schema/storage 接口。

## 关键接口（含 manager）

### StorageBase（`include/dreamdb/storage/storage_base.h`）
- `insert/remove_by_id/update_by_id/get_by_id/query`
- `size/empty/contains/clear`

### MemoryStorage（`include/dreamdb/storage/memory_storage.h`）
- 基于 `unordered_map` 的内存实现，完全符合 `StorageBase` 接口。

### Segment（`include/dreamdb/storage/segment.h`）
- 属性：`id`、`status (GROWING/SEALED/FLUSHED/DROPPED)`、`created_at`、`sealed_at`、`storage_`。
- 状态：`seal()` 封存；`set_status/get_status`。
- CRUD 与查询：`insert/remove_by_id/update_by_id/get_by_id/query`。
- 统计：`size/empty`。

### SegmentManager（待完善，`include/dreamdb/storage/segment_manager.h`）
- 创建/获取/删除：`create_segment()`, `get_segment(id)`, `remove_segment(id)`, `contains(id)`。
- 状态与选择：`get_growing_segment()`, `get_segments_by_status(status)`, `seal_segment(id)`。
- 遍历与统计：`for_each_segment(cb)`, `count_by_status(status)`, `size/empty`。

### Collection（`include/dreamdb/schema/collection.h`）
- 元数据：`name`、`schema`、`next_id`、`segment_manager`、`active_segment`。
- 实体构造：`create_entity()`（分配内部自增 ID 和字段槽位）。
- CRUD/查询：通过 `active_segment` 调用段接口：`insert/remove/update/query`（目前仅插入实现）。

### CollectionManager（`include/dreamdb/schema/collection_manager.h`）
- 管理集合：`create_collection(name, schema)`, `drop_collection(name)`, `get_collection(name)`。
- 路由：对外暴露集合级 CRUD/查询，内部查找对应 `Collection` 并转发。

### Database / DatabaseManager（`include/dreamdb/schema/database*.h`）
- 管理数据库元数据与集合管理器，提供数据库级的创建、删除、查找、委派集合操作。

### Query 相关（`include/dreamdb/query/*.h`）
- `Query` 持有 `Condition`、`Order`、`Limit` 等条件；执行时由段或集合层遍历存储实现。

## 用户操作典型数据流

### 1) 创建数据库与集合
1. 用户通过 `DatabaseManager::create_database(name)` 获取 `Database`。
2. 用户调用 `Database::create_collection(name, schema)`，内部使用 `CollectionManager`。
3. `CollectionManager` 创建 `Collection`，注入一个新的 `SegmentManager`，并创建首个 `active_segment`（GROWING）。

### 2) 插入数据
1. 用户调用 `Collection::create_entity()` 构造实体并填充字段。
2. `Collection::insert(entity)` 调用 `active_segment_->insert(entity)`。
3. `Segment` 检查状态（应为 GROWING）后，将请求委托给 `storage_`（默认 `MemoryStorage`），完成写入。

### 3) 查询数据
1. 用户调用 `Collection::query(Query)`。
2. Collection 选择需要参与的段（当前实现可直接用 `active_segment`；未来可遍历所有段）。
3. 对每个段调用 `segment->query(query)`，由段内部 `storage_->query` 执行过滤、排序、限制；合并结果返回。

### 4) 更新 / 删除
1. 用户调用 `Collection::update(id, fields)` 或 `remove(id)`。
2. Collection 定位目标段（可依据分片/索引策略；当前可以默认 active 段或未来建立 ID→段映射）。
3. 段检查状态（SEALED 段不应接受写），再转发给底层 `storage_`。

### 5) 段生命周期管理
1. 当段满足大小/时间阈值时，`SegmentManager::seal_segment(id)` 将段置为 SEALED，并记录 `sealed_at`。
2. 未来可由 `SegmentManager` 触发刷新（FLUSHED）、删除（DROPPED）或创建新 GROWING 段并让 Collection 轮转 `active_segment`。

### 6) 删除集合 / 数据库
1. `CollectionManager::drop_collection(name)` 释放集合与其段。
2. `DatabaseManager::drop_database(name)` 级联释放其集合与段资源。

## 后续可完善点
- 在 `Segment` 的 CRUD 前增加状态检查，SEALED 段拒绝写入。
- `SegmentManager` 补全接口实现：段查找、状态过滤、计数、遍历。
- Collection 在插入时检测 `active_segment` 大小，必要时通过 `SegmentManager` 轮转新段。
- Query 聚合多段结果并做全局排序/限制。


