#include "dreamdb/schema/collection.h"

#include <stdexcept>
#include <type_traits>

namespace dreamdb
{

// 保留字段名
static constexpr const char* ID_FIELD_NAME = "id";
static constexpr const char* VECTOR_FIELD_NAME = "vector";

// 固定字段索引
static constexpr std::size_t ID_FIELD_INDEX = 0;
static constexpr std::size_t VECTOR_FIELD_INDEX = 1;

Collection::Collection(
    const std::string & name, 
    const std::vector<Field> & user_schema,
    int vector_dimension,
    MetricType metric_type
)
    : name(name),
      user_schema(user_schema),
      vector_dimension(vector_dimension),
      metric_type(metric_type)
{
    // 验证向量维度
    if (vector_dimension <= 0) {
        throw std::invalid_argument("Vector dimension must be greater than 0");
    }
    
    // 验证用户 schema 不包含保留字段名
    for (const auto& field : user_schema) {
        const std::string& field_name = field.get_name();
        if (field_name == ID_FIELD_NAME || field_name == VECTOR_FIELD_NAME) {
            throw std::invalid_argument(
                "Field name '" + field_name + "' is reserved and cannot be used"
            );
        }
    }
    
    // 构建完整 schema：id + vector + 用户字段
    full_schema.clear();
    
    // 添加 id 字段（索引 0）
    full_schema.push_back(
        Field::create_int64_field(ID_FIELD_NAME, false, true, "", NullType(), false)
    );
    
    // 添加 vector 字段（索引 1）
    full_schema.push_back(
        Field::create_float_vector_field(VECTOR_FIELD_NAME, false, false, "", NullType())
    );
    
    // 添加用户定义的字段（从索引 2 开始）
    full_schema.insert(full_schema.end(), user_schema.begin(), user_schema.end());
    
    // 初始化字段索引映射
    initialize_field_index_map();
}

const std::string & Collection::get_name() const
{
    return name;
}

const std::vector<Field> & Collection::get_schema() const
{
    return full_schema;
}

const std::vector<Field> & Collection::get_user_schema() const
{
    return user_schema;
}

const Field & Collection::get_field(const std::string & field_name) const
{
    auto it = field_index_map.find(field_name);
    if (it == field_index_map.end()) {
        throw std::invalid_argument("Field '" + field_name + "' does not exist");
    }
    return full_schema[it->second];
}

const Field & Collection::get_field(std::size_t index) const
{
    if (index >= full_schema.size()) {
        throw std::out_of_range("Field index out of range");
    }
    return full_schema[index];
}

std::size_t Collection::get_field_index(const std::string & field_name) const
{
    auto it = field_index_map.find(field_name);
    if (it == field_index_map.end()) {
        throw std::invalid_argument("Field '" + field_name + "' does not exist");
    }
    return it->second;
}

std::size_t Collection::field_count() const
{
    return full_schema.size();
}

bool Collection::has_field(const std::string & field_name) const
{
    return field_index_map.find(field_name) != field_index_map.end();
}

Entity & Collection::create_entity()
{
    // 自动分配自增 ID
    std::int64_t internal_id = next_id++;
    
    // 创建实体并存储
    auto [it, inserted] = entities_.emplace(internal_id, Entity(full_schema.size()));
    Entity& entity = it->second;
    
    // 设置 id 字段（索引 0）
    entity.set_value(ID_FIELD_INDEX, internal_id);
    
    // vector 字段保持为 NullType（需要用户后续设置）
    
    // 使用默认值初始化用户字段
    for (std::size_t i = 2; i < full_schema.size(); ++i) {
        const auto& field = full_schema[i];
        const auto& default_value = field.get_default_value();
        
        if (!std::holds_alternative<NullType>(default_value)) {
            entity.set_value(i, std::visit([](const auto& val) -> EntityValue {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, char>) {
                    return std::string(1, val);
                }
                else {
                    return EntityValue(val);
                }
            }, default_value));
        }
    }
    
    return entity;
}

Entity & Collection::create_entity(const std::string & id)
{
    // 检查 string ID 是否已存在
    if (entity_id_map.find(id) != entity_id_map.end()) {
        throw std::invalid_argument("String ID '" + id + "' already exists");
    }
    
    // 创建实体
    Entity& entity = create_entity();
    
    // 获取已分配的内部 ID
    std::int64_t internal_id = entity.get_value_as<std::int64_t>(ID_FIELD_INDEX);
    
    // 建立映射关系
    entity_id_map[id] = internal_id;
    id_entity_map[internal_id] = id;
    
    return entity;
}

const std::string & Collection::get_entity_string_id(const Entity & entity) const
{
    std::int64_t internal_id = get_entity_id(entity);
    auto it = id_entity_map.find(internal_id);
    if (it == id_entity_map.end()) {
        throw std::runtime_error("Entity does not have a string ID");
    }
    return it->second;  // 返回映射表中的引用，生命周期由 Collection 管理
}

std::int64_t Collection::get_entity_id(const Entity & entity) const
{
    return entity.get_value_as<std::int64_t>(ID_FIELD_INDEX);
}

void Collection::set_entity_field(Entity & entity, const std::string & field_name, const EntityValue & value) const
{
    // 禁止直接修改 id 字段
    if (field_name == ID_FIELD_NAME) {
        throw std::invalid_argument("Cannot modify 'id' field directly");
    }
    
    std::size_t index = get_field_index(field_name);
    const auto& field = full_schema[index];
    
    // 验证类型
    if (!std::holds_alternative<NullType>(value)) {
        if (!validate_field_type(field, value)) {
            throw std::invalid_argument(
                "Field '" + field_name + "' type mismatch"
            );
        }
        
        // 如果是向量字段，验证维度
        if (field.get_type() == FieldType::FLOAT_VECTOR) {
            if (std::holds_alternative<std::vector<float>>(value)) {
                const auto& vec = std::get<std::vector<float>>(value);
                if (!validate_vector_dimension(vec)) {
                    throw std::invalid_argument(
                        "Field '" + field_name + "' vector dimension mismatch"
                    );
                }
            }
        }
    }
    
    // 检查非空约束
    if (!field.get_is_nullable() && std::holds_alternative<NullType>(value)) {
        throw std::invalid_argument(
            "Field '" + field_name + "' is not nullable"
        );
    }
    
    entity.set_value(index, value);
}

const EntityValue & Collection::get_entity_field(const Entity & entity, const std::string & field_name) const
{
    std::size_t index = get_field_index(field_name);
    return entity.get_value(index);
}

void Collection::set_entity_vector(Entity & entity, const std::string & field_name, const std::vector<float> & vector) const
{
    std::size_t index = get_field_index(field_name);
    const auto& field = full_schema[index];
    
    // 检查是否为向量字段
    if (field.get_type() != FieldType::FLOAT_VECTOR) {
        throw std::invalid_argument(
            "Field '" + field_name + "' is not a vector field"
        );
    }
    
    // 验证维度
    if (!validate_vector_dimension(vector)) {
        throw std::invalid_argument(
            "Field '" + field_name + "' vector dimension mismatch"
        );
    }
    
    entity.set_value(index, vector);
}

const std::vector<float> & Collection::get_entity_vector(const Entity & entity, const std::string & field_name) const
{
    std::size_t index = get_field_index(field_name);
    const auto& field = full_schema[index];
    
    // 检查是否为向量字段
    if (field.get_type() != FieldType::FLOAT_VECTOR) {
        throw std::invalid_argument(
            "Field '" + field_name + "' is not a vector field"
        );
    }
    
    const auto& value = entity.get_value(index);
    return std::get<std::vector<float>>(value);
}

std::int64_t Collection::get_internal_id(const std::string & string_id) const
{
    auto it = entity_id_map.find(string_id);
    if (it == entity_id_map.end()) {
        throw std::invalid_argument("String ID '" + string_id + "' does not exist");
    }
    return it->second;
}

std::optional<std::string> Collection::get_string_id(std::int64_t internal_id) const
{
    auto it = id_entity_map.find(internal_id);
    if (it == id_entity_map.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool Collection::has_string_id(const std::string & string_id) const
{
    return entity_id_map.find(string_id) != entity_id_map.end();
}

Entity & Collection::get_entity(std::int64_t internal_id)
{
    auto it = entities_.find(internal_id);
    if (it == entities_.end()) {
        throw std::invalid_argument("Entity with internal ID " + std::to_string(internal_id) + " does not exist");
    }
    return it->second;
}

const Entity & Collection::get_entity(std::int64_t internal_id) const
{
    auto it = entities_.find(internal_id);
    if (it == entities_.end()) {
        throw std::invalid_argument("Entity with internal ID " + std::to_string(internal_id) + " does not exist");
    }
    return it->second;
}

Entity & Collection::get_entity(const std::string & string_id)
{
    std::int64_t internal_id = get_internal_id(string_id);
    return get_entity(internal_id);
}

const Entity & Collection::get_entity(const std::string & string_id) const
{
    std::int64_t internal_id = get_internal_id(string_id);
    return get_entity(internal_id);
}

bool Collection::has_entity(std::int64_t internal_id) const
{
    return entities_.find(internal_id) != entities_.end();
}

bool Collection::has_entity(const std::string & string_id) const
{
    return has_string_id(string_id) && has_entity(get_internal_id(string_id));
}

void Collection::delete_entity(std::int64_t internal_id)
{
    auto it = entities_.find(internal_id);
    if (it == entities_.end()) {
        throw std::invalid_argument("Entity with internal ID " + std::to_string(internal_id) + " does not exist");
    }
    
    // 删除 ID 映射
    auto id_it = id_entity_map.find(internal_id);
    if (id_it != id_entity_map.end()) {
        entity_id_map.erase(id_it->second);
        id_entity_map.erase(id_it);
    }
    
    // 删除实体
    entities_.erase(it);
}

void Collection::delete_entity(const std::string & string_id)
{
    std::int64_t internal_id = get_internal_id(string_id);
    delete_entity(internal_id);
}

std::size_t Collection::entity_count() const
{
    return entities_.size();
}

void Collection::initialize_field_index_map()
{
    field_index_map.clear();
    
    for (std::size_t i = 0; i < full_schema.size(); ++i) {
        const auto& field = full_schema[i];
        const std::string& name = field.get_name();
        
        // 检查字段名是否重复
        if (field_index_map.find(name) != field_index_map.end()) {
            throw std::invalid_argument("Duplicate field name: " + name);
        }
        
        field_index_map[name] = i;
    }
}

bool Collection::validate_field_type(const Field& field, const EntityValue& value) const
{
    return std::visit([&field](const auto& val) -> bool {
        using T = std::decay_t<decltype(val)>;
        
        switch (field.get_type()) {
            case FieldType::INT64:
                return std::is_same_v<T, std::int64_t>;
            case FieldType::FLOAT:
                return std::is_same_v<T, float>;
            case FieldType::DOUBLE:
                return std::is_same_v<T, double>;
            case FieldType::CHAR:
            case FieldType::VARCHAR:
                return std::is_same_v<T, std::string>;
            case FieldType::BOOLEAN:
                return std::is_same_v<T, bool>;
            case FieldType::TIMESTAMP:
                return std::is_same_v<T, std::int64_t>;
            case FieldType::FLOAT_VECTOR:
                return std::is_same_v<T, std::vector<float>>;
            default:
                return false;
        }
    }, value);
}

bool Collection::validate_vector_dimension(const std::vector<float>& vector) const
{
    if (vector_dimension <= 0) {
        return false;
    }

    return static_cast<std::size_t>(vector_dimension) == vector.size();
}

} // namespace dreamdb
