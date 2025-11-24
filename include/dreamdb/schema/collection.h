#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <optional>

#include "dreamdb/schema/field.h"
#include "dreamdb/schema/entity.h"
#include "dreamdb/common/type.h"

namespace dreamdb
{

/**
 * @brief 集合类（表）
 * 管理 schema 定义和实体记录
 */
class Collection
{
public:
    /**
     * @brief 构造函数
     * @param name 集合名称
     * @param user_schema 用户定义的字段定义列表
     * @param vector_dimension 向量维度
     * @param metric_type 向量距离度量类型
     */
    Collection(
        const std::string & name, 
        const std::vector<Field> & user_schema,
        int vector_dimension,
        MetricType metric_type
    );

    ~Collection() = default;

public:

    /**
     * @brief 获取集合名称
     * @return 集合名称
     */
    const std::string & get_name() const;

    /**
     * @brief 获取完整字段定义列表
     * @details 包括用户定义的字段定义和向量字段定义
     * @return 完整字段定义列表
     */
    const std::vector<Field> & get_schema() const;

    /**
     * @brief 获取用户定义的字段定义列表
     * @return 用户定义的字段定义列表
     */
    const std::vector<Field> & get_user_schema() const;

    /**
     * @brief 获取字段定义
     * @param field_name 字段名称
     * @return 字段定义
     * @throw std::invalid_argument 如果字段不存在
     */
    const Field & get_field(const std::string & field_name) const;

    /**
     * @brief 获取字段定义
     * @param index 字段索引
     * @return 字段定义
     * @throw std::out_of_range 如果索引越界
     */
    const Field & get_field(std::size_t index) const;

    /**
     * @brief 获取字段索引
     * @param field_name 字段名称
     * @return 字段索引
     * @throw std::invalid_argument 如果字段不存在
     */
    std::size_t get_field_index(const std::string & field_name) const;

    /**
     * @brief 获取字段数量
     * @return 字段数量
     */
    std::size_t field_count() const;

    /**
     * @brief 检查字段是否存在
     * @param field_name 字段名称
     * @return 如果字段存在返回 true
     */
    bool has_field(const std::string & field_name) const;

    /**
     * @brief 创建并插入新实体
     * @details 自动分配自增 ID，并存储到 Collection 中
     * @return 新创建的实体的引用
     */
    Entity & create_entity();

    /**
     * @brief 创建并插入新实体
     * @param id 向量 ID
     * @return 新创建的实体的引用
     */
    Entity & create_entity(const std::string & id);

    /**
     * @brief 获取实体向量 ID
     * @param entity 实体
     * @return 向量 ID
     */
    const std::string & get_entity_string_id(const Entity & entity) const;

    /**
     * @brief 获取实体自增 ID
     * @param entity 实体
     * @return 自增 ID
     */
    std::int64_t get_entity_id(const Entity & entity) const;

    /**
     * @brief 设置实体字段值
     * @param entity 实体
     * @param field_name 字段名称
     * @param value 字段值
     * @throw std::invalid_argument 如果字段不存在或类型不匹配
     */
    void set_entity_field(Entity & entity, const std::string & field_name, const EntityValue & value) const;
    
    /**
     * @brief 获取实体字段值
     * @param entity 实体
     * @param field_name 字段名称
     * @return 字段值
     * @throw std::invalid_argument 如果字段不存在
     */
    const EntityValue & get_entity_field(const Entity & entity, const std::string & field_name) const;
    
    /**
     * @brief 设置实体向量字段
     * @param entity 实体
     * @param field_name 向量字段名称
     * @param vector 向量数据
     * @throw std::invalid_argument 如果字段不存在、不是向量类型或维度不匹配
     */
    void set_entity_vector(Entity & entity, const std::string & field_name, const std::vector<float> & vector) const;
    
    /**
     * @brief 获取实体向量字段
     * @param entity 实体
     * @param field_name 向量字段名称
     * @return 向量数据
     * @throw std::invalid_argument 如果字段不存在或不是向量类型
     */
    const std::vector<float> & get_entity_vector(const Entity & entity, const std::string & field_name) const;

    /**
     * @brief 获取内部 ID
     * @param string_id 向量 ID
     * @return 内部 ID
     */
    std::int64_t get_internal_id(const std::string & string_id) const;

    /**
     * @brief 获取向量 ID
     * @param internal_id 内部 ID
     * @return 向量 ID
     */
    std::optional<std::string> get_string_id(std::int64_t internal_id) const;

    /**
     * @brief 检查向量 ID 是否存在
     * @param string_id 向量 ID
     * @return 如果向量 ID 存在返回 true
     */
    bool has_string_id(const std::string & string_id) const;

    /**
     * @brief 获取实体（通过内部 ID）
     * @param internal_id 内部 ID
     * @return 实体的引用
     * @throw std::invalid_argument 如果实体不存在
     */
    Entity & get_entity(std::int64_t internal_id);

    /**
     * @brief 获取实体（通过内部 ID，const 版本）
     * @param internal_id 内部 ID
     * @return 实体的常量引用
     * @throw std::invalid_argument 如果实体不存在
     */
    const Entity & get_entity(std::int64_t internal_id) const;

    /**
     * @brief 获取实体（通过向量 ID）
     * @param string_id 向量 ID
     * @return 实体的引用
     * @throw std::invalid_argument 如果实体不存在
     */
    Entity & get_entity(const std::string & string_id);

    /**
     * @brief 获取实体（通过向量 ID，const 版本）
     * @param string_id 向量 ID
     * @return 实体的常量引用
     * @throw std::invalid_argument 如果实体不存在
     */
    const Entity & get_entity(const std::string & string_id) const;

    /**
     * @brief 检查实体是否存在（通过内部 ID）
     * @param internal_id 内部 ID
     * @return 如果实体存在返回 true
     */
    bool has_entity(std::int64_t internal_id) const;

    /**
     * @brief 检查实体是否存在（通过向量 ID）
     * @param string_id 向量 ID
     * @return 如果实体存在返回 true
     */
    bool has_entity(const std::string & string_id) const;

    /**
     * @brief 删除实体（通过内部 ID）
     * @param internal_id 内部 ID
     * @throw std::invalid_argument 如果实体不存在
     */
    void delete_entity(std::int64_t internal_id);

    /**
     * @brief 删除实体（通过向量 ID）
     * @param string_id 向量 ID
     * @throw std::invalid_argument 如果实体不存在
     */
    void delete_entity(const std::string & string_id);

    /**
     * @brief 获取实体数量
     * @return 实体数量
     */
    std::size_t entity_count() const;

private:
    /**
     * @brief 初始化字段索引映射
     */
    void initialize_field_index_map();
    
    /**
     * @brief 验证字段值类型是否匹配
     * @param field 字段定义
     * @param value 字段值
     * @return 如果类型匹配返回 true
     */
    bool validate_field_type(const Field& field, const EntityValue& value) const;
    
    /**
     * @brief 验证向量维度是否匹配
     * @param field 字段定义
     * @param vector 向量数据
     * @return 如果维度匹配返回 true
     */
    bool validate_vector_dimension(const std::vector<float>& vector) const;

private:
    std::string name;                                    // 集合名称
    std::vector<Field> user_schema;                      // 用户定义的字段定义列表
    std::vector<Field> full_schema;                      // 完整字段定义列表
    int vector_dimension;                                // 向量维度
    MetricType metric_type;                              // 向量距离度量类型
    std::unordered_map<std::string, std::size_t> field_index_map;  // 字段名到索引的映射

    // ID 映射表
    std::unordered_map<std::int64_t, std::string> id_entity_map;    // 自增 ID 到 向量 ID 的映射表
    std::unordered_map<std::string, std::int64_t> entity_id_map;    // 向量 ID 到 自增 ID 的映射表
    std::int64_t next_id = 1;                            // 自增 ID 生成器
    
    // 实体存储
    std::unordered_map<std::int64_t, Entity> entities_;  // 按内部 ID 存储实体
};

} // namespace dreamdb

