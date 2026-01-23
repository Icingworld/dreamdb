#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

#include "dreamdb/catalog/catalog_reader.h"
#include "dreamdb/catalog/catalog_writer.h"

namespace dreamdb::catalog
{

/**
 * @brief 数据库目录
 */
class Catalog : public CatalogReader, public CatalogWriter
{
public:
    Catalog() noexcept = default;

    ~Catalog() noexcept override = default;
};

} // namespace dreamdb::catalog
