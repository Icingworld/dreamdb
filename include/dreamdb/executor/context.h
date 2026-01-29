#pragma once

namespace dreamdb::catalog
{

class CatalogReader;
class CatalogWriter;

} // namespace dreamdb::catalog

namespace dreamdb::storage
{

class Storage;

} // namespace dreamdb::storage

namespace dreamdb::executor
{

struct ExecutionContext
{
};

/**
 * @brief 只读执行上下文
 */
struct ExecutionContextReadOnly : public ExecutionContext
{
    const dreamdb::catalog::CatalogReader & catalog_reader;  // 目录读取器
    const dreamdb::storage::Storage & storage;               // 存储引擎
};

/**
 * @brief 可写执行上下文
 */
struct ExecutionContextWritable : public ExecutionContext
{
    dreamdb::catalog::CatalogWriter & catalog_writer;  // 目录写入器
    dreamdb::storage::Storage & storage;               // 存储引擎
};

} // namespace dreamdb::executor
