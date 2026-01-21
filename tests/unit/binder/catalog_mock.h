#pragma once

#include <gmock/gmock.h>

#include "dreamdb/catalog/catalog.h"

/**
 * @brief 模拟 Catalog 类
 */
class CatalogMock : public dreamdb::catalog::Catalog
{
public:
    /** ========== CatalogReader ========== */

    MOCK_METHOD(
        std::optional<dreamdb::common::database_id_t>,
        resolve_database,
        (const std::string & database_name),
        (const, override)
    );

    MOCK_METHOD(
        std::optional<dreamdb::common::collection_id_t>,
        resolve_collection,
        (const dreamdb::common::database_id_t database_id, const std::string & collection_name),
        (const, override)
    );

    MOCK_METHOD(
        std::optional<dreamdb::catalog::column_info_t>,
        resolve_column,
        (const dreamdb::common::collection_id_t collection_id, const std::string & column_name),
        (const, override)
    );

    MOCK_METHOD(
        std::optional<dreamdb::common::index_id_t>,
        resolve_index,
        (const dreamdb::common::collection_id_t collection_id, const std::string & index_name),
        (const, override)
    );

    MOCK_METHOD(
        std::optional<dreamdb::common::vindex_id_t>,
        resolve_vindex,
        (const dreamdb::common::collection_id_t collection_id, const std::string & vindex_name),
        (const, override)
    );

    MOCK_METHOD(
        std::vector<dreamdb::catalog::column_info_t>,
        get_columns,
        (const dreamdb::common::collection_id_t collection_id),
        (const, override)
    );

    /** ========== CatalogWriter ========== */

    MOCK_METHOD(
        void,
        create_database,
        (const std::string & database_name),
        (override)
    );

    MOCK_METHOD(
        void,
        drop_database,
        (const dreamdb::common::database_id_t database_id),
        (override)
    );
};
