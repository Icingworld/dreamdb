#pragma once

#include <string>

namespace dreamdb::catalog
{

/**
 * @brief 数据库目录写入器
 */
class CatalogWriter
{
public:
    virtual ~CatalogWriter() noexcept = default;
    
public:
    /**
     * @brief 创建数据库
     * @param database_name 数据库名称
     */
    virtual void create_database(const std::string & database_name) = 0;

    /**
     * @brief 删除数据库
     * @param database_name 数据库名称
     */
    virtual void drop_database(const std::string & database_name) = 0;
};

} // namespace dreamdb::catalog
