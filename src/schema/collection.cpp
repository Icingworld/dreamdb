#include "dreamdb/schema/collection.h"

#include <stdexcept>

#include "dreamdb/storage/segment_manager.h"
#include "dreamdb/storage/segment.h"

namespace dreamdb
{

namespace
{

MutationResult make_error(const std::string & message)
{
    MutationResult result;
    result.success = false;
    result.error_message = message;
    return result;
}

} // namespace

Collection::Collection(
    const std::string & name,
    const std::vector<Field> & schema,
    SegmentManager & segment_manager
)
    : name(name)
    , schema(schema)
    , next_id(1)
    , segment_manager(segment_manager)
{
}

void Collection::set_name(const std::string & name)
{
    this->name = name;
}

void Collection::set_schema(const std::vector<Field> & schema)
{
    this->schema = schema;
}

const std::string & Collection::get_name() const
{
    return name;
}

const std::vector<Field> & Collection::get_schema() const
{
    return schema;
}

Entity Collection::create_entity()
{
    // 预分配字段数量
    Entity entity(next_id++, schema.size());

    return entity;
}

MutationResult Collection::insert(const Entity & entity)
{
    if (entity.field_count() != schema.size()) {
        return make_error("Entity field count mismatch");
    }

    auto * segment = segment_manager.get_or_create_segment(name, schema.size());
    if (!segment) {
        return make_error("Failed to acquire segment");
    }

    auto * storage = segment->get_storage();
    if (!storage) {
        return make_error("Segment has no storage backend");
    }

    return storage->insert(entity);
}

MutationResult Collection::remove(const std::string & key, const std::string & value)
{
    const auto field_index = find_field_index(key);
    if (!field_index) {
        return make_error("Field '" + key + "' not found in schema");
    }

    FieldValue literal;
    try {
        literal = parse_literal(schema[*field_index], value);
    }
    catch (const std::exception & e) {
        return make_error(e.what());
    }

    MutationResult result;
    auto segments = segment_manager.get_segments(name);
    for (auto * segment : segments) {
        if (!segment) {
            continue;
        }

        auto * storage = segment->get_storage();
        if (!storage) {
            continue;
        }

        const auto segment_result = storage->remove_by_field(*field_index, literal);
        if (segment_result.success) {
            result.success = true;
            result.affected_count += segment_result.affected_count;
        }
    }

    if (!result.success) {
        result.error_message = "No matching entity found";
    }

    return result;
}

MutationResult Collection::upsert(const std::string & key, const std::string & value, const Entity & entity)
{
    if (entity.field_count() != schema.size()) {
        return make_error("Entity field count mismatch");
    }

    const auto field_index = find_field_index(key);
    if (!field_index) {
        return make_error("Field '" + key + "' not found in schema");
    }

    FieldValue literal;
    try {
        literal = parse_literal(schema[*field_index], value);
    }
    catch (const std::exception & e) {
        return make_error(e.what());
    }

    auto segments = segment_manager.get_segments(name);
    for (auto * segment : segments) {
        if (!segment) {
            continue;
        }

        auto * storage = segment->get_storage();
        if (!storage) {
            continue;
        }

        const auto segment_result = storage->upsert_by_field(*field_index, literal, entity);
        if (segment_result.success) {
            return segment_result;
        }
    }

    // 如果没有匹配的实体，则插入新实体
    return insert(entity);
}

std::optional<std::size_t> Collection::find_field_index(const std::string & key) const
{
    for (std::size_t i = 0; i < schema.size(); ++i) {
        if (schema[i].get_name() == key) {
            return i;
        }
    }

    return std::nullopt;
}

FieldValue Collection::parse_literal(const Field & field, const std::string & literal) const
{
    switch (field.get_type()) {
        case FieldType::INT8: return static_cast<std::int8_t>(std::stoi(literal));
        case FieldType::INT16: return static_cast<std::int16_t>(std::stoi(literal));
        case FieldType::INT32: return static_cast<std::int32_t>(std::stol(literal));
        case FieldType::INT64:
        case FieldType::TIMESTAMP: return static_cast<std::int64_t>(std::stoll(literal));
        case FieldType::FLOAT: return std::stof(literal);
        case FieldType::DOUBLE: return std::stod(literal);
        case FieldType::CHAR:
            if (literal.empty()) {
                throw std::invalid_argument("Literal for CHAR cannot be empty");
            }
            return literal[0];
        case FieldType::VARCHAR:
        case FieldType::ENUM: return literal;
        case FieldType::BOOLEAN:
            if (literal == "true" || literal == "1") {
                return true;
            }
            if (literal == "false" || literal == "0") {
                return false;
            }
            throw std::invalid_argument("Invalid literal for BOOLEAN field: " + literal);
        case FieldType::FLOAT_VECTOR:
            throw std::invalid_argument("Literal parsing for FLOAT_VECTOR is not supported");
        default:
            throw std::invalid_argument("Unsupported field type literal parsing");
    }
}

} // namespace dreamdb
