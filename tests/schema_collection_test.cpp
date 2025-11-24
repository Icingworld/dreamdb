#include <dreamdb/schema/collection.h>
#include <cassert>
#include <iostream>

#include "common.h"

using namespace dreamdb;

// Test constructor
void test_constructor()
{
    std::cout << "Testing constructor..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));
    schema.push_back(Field::create_int64_field("age"));

    Collection collection("test_collection", schema, 128, MetricType::L2);
    
    TEST_ASSERT(collection.get_name() == "test_collection", "Collection name should match");
    TEST_ASSERT(collection.field_count() == 4, "Collection should have 4 fields (id, vector, name, age)");
    TEST_ASSERT(collection.get_user_schema().size() == 2, "User schema should have 2 fields");
    TEST_ASSERT(collection.entity_count() == 0, "New collection should have 0 entities");

    std::cout << "  Constructor test passed" << std::endl;
}

// Test constructor validation
void test_constructor_validation()
{
    std::cout << "Testing constructor validation..." << std::endl;

    std::vector<Field> schema;

    // Test invalid vector dimension
    TEST_EXCEPTION(
        Collection("test", schema, 0, MetricType::L2),
        std::invalid_argument,
        "Constructor with zero vector dimension should throw exception"
    );

    TEST_EXCEPTION(
        Collection("test", schema, -1, MetricType::L2),
        std::invalid_argument,
        "Constructor with negative vector dimension should throw exception"
    );

    // Test reserved field name "id"
    std::vector<Field> schema_with_id;
    schema_with_id.push_back(Field::create_string_field("id"));
    TEST_EXCEPTION(
        Collection("test", schema_with_id, 128, MetricType::L2),
        std::invalid_argument,
        "Constructor with reserved field name 'id' should throw exception"
    );

    // Test reserved field name "vector"
    std::vector<Field> schema_with_vector;
    schema_with_vector.push_back(Field::create_string_field("vector"));
    TEST_EXCEPTION(
        Collection("test", schema_with_vector, 128, MetricType::L2),
        std::invalid_argument,
        "Constructor with reserved field name 'vector' should throw exception"
    );

    std::cout << "  Constructor validation test passed" << std::endl;
}

// Test schema access
void test_schema_access()
{
    std::cout << "Testing schema access..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));
    schema.push_back(Field::create_int64_field("age"));

    Collection collection("test", schema, 128, MetricType::L2);

    // Test get_schema
    const auto & full_schema = collection.get_schema();
    TEST_ASSERT(full_schema.size() == 4, "Full schema should have 4 fields");
    TEST_ASSERT(full_schema[0].get_name() == "id", "First field should be 'id'");
    TEST_ASSERT(full_schema[1].get_name() == "vector", "Second field should be 'vector'");
    TEST_ASSERT(full_schema[2].get_name() == "name", "Third field should be 'name'");
    TEST_ASSERT(full_schema[3].get_name() == "age", "Fourth field should be 'age'");

    // Test get_user_schema
    const auto & user_schema = collection.get_user_schema();
    TEST_ASSERT(user_schema.size() == 2, "User schema should have 2 fields");
    TEST_ASSERT(user_schema[0].get_name() == "name", "First user field should be 'name'");
    TEST_ASSERT(user_schema[1].get_name() == "age", "Second user field should be 'age'");

    std::cout << "  Schema access test passed" << std::endl;
}

// Test field access
void test_field_access()
{
    std::cout << "Testing field access..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));
    schema.push_back(Field::create_int64_field("age"));

    Collection collection("test", schema, 128, MetricType::L2);

    // Test get_field by name
    const auto & id_field = collection.get_field("id");
    TEST_ASSERT(id_field.get_name() == "id", "Field name should match");
    TEST_ASSERT(id_field.get_type() == FieldType::INT64, "ID field should be INT64");

    const auto & vector_field = collection.get_field("vector");
    TEST_ASSERT(vector_field.get_name() == "vector", "Vector field name should match");
    TEST_ASSERT(vector_field.get_is_vector() == true, "Vector field should be vector type");

    const auto & name_field = collection.get_field("name");
    TEST_ASSERT(name_field.get_name() == "name", "Name field should match");

    // Test get_field by index
    const auto & field0 = collection.get_field(0);
    TEST_ASSERT(field0.get_name() == "id", "Field at index 0 should be 'id'");

    const auto & field1 = collection.get_field(1);
    TEST_ASSERT(field1.get_name() == "vector", "Field at index 1 should be 'vector'");

    // Test get_field_index
    TEST_ASSERT(collection.get_field_index("id") == 0, "ID field index should be 0");
    TEST_ASSERT(collection.get_field_index("vector") == 1, "Vector field index should be 1");
    TEST_ASSERT(collection.get_field_index("name") == 2, "Name field index should be 2");

    // Test has_field
    TEST_ASSERT(collection.has_field("id") == true, "Should have 'id' field");
    TEST_ASSERT(collection.has_field("vector") == true, "Should have 'vector' field");
    TEST_ASSERT(collection.has_field("name") == true, "Should have 'name' field");
    TEST_ASSERT(collection.has_field("nonexistent") == false, "Should not have 'nonexistent' field");

    // Test exceptions
    TEST_EXCEPTION(
        collection.get_field("nonexistent"),
        std::invalid_argument,
        "get_field with nonexistent field should throw exception"
    );

    TEST_EXCEPTION(
        collection.get_field(100),
        std::out_of_range,
        "get_field with out of range index should throw exception"
    );

    TEST_EXCEPTION(
        collection.get_field_index("nonexistent"),
        std::invalid_argument,
        "get_field_index with nonexistent field should throw exception"
    );

    std::cout << "  Field access test passed" << std::endl;
}

// Test entity creation
void test_entity_creation()
{
    std::cout << "Testing entity creation..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));
    schema.push_back(Field::create_int64_field("age"));

    Collection collection("test", schema, 128, MetricType::L2);

    // Test create_entity() - auto ID
    Entity & entity1 = collection.create_entity();
    TEST_ASSERT(collection.entity_count() == 1, "Collection should have 1 entity");
    TEST_ASSERT(collection.get_entity_id(entity1) == 1, "First entity should have ID 1");

    Entity & entity2 = collection.create_entity();
    TEST_ASSERT(collection.entity_count() == 2, "Collection should have 2 entities");
    TEST_ASSERT(collection.get_entity_id(entity2) == 2, "Second entity should have ID 2");

    // Test create_entity(string_id)
    Entity & entity3 = collection.create_entity("user_123");
    TEST_ASSERT(collection.entity_count() == 3, "Collection should have 3 entities");
    TEST_ASSERT(collection.get_entity_id(entity3) == 3, "Third entity should have ID 3");
    TEST_ASSERT(collection.get_entity_string_id(entity3) == "user_123", "Entity should have string ID");

    // Test duplicate string ID
    TEST_EXCEPTION(
        collection.create_entity("user_123"),
        std::invalid_argument,
        "Creating entity with duplicate string ID should throw exception"
    );

    std::cout << "  Entity creation test passed" << std::endl;
}

// Test entity field operations
void test_entity_field_operations()
{
    std::cout << "Testing entity field operations..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));
    schema.push_back(Field::create_int64_field("age", false));  // not nullable

    Collection collection("test", schema, 128, MetricType::L2);

    Entity & entity = collection.create_entity();

    // Test set_entity_field
    collection.set_entity_field(entity, "name", std::string("Alice"));
    collection.set_entity_field(entity, "age", std::int64_t(30));

    // Test get_entity_field
    const auto & name_value = collection.get_entity_field(entity, "name");
    TEST_ASSERT(std::holds_alternative<std::string>(name_value), "Name value should be string");
    TEST_ASSERT(std::get<std::string>(name_value) == "Alice", "Name value should match");

    const auto & age_value = collection.get_entity_field(entity, "age");
    TEST_ASSERT(std::holds_alternative<std::int64_t>(age_value), "Age value should be int64");
    TEST_ASSERT(std::get<std::int64_t>(age_value) == 30, "Age value should match");

    // Test cannot modify id field
    TEST_EXCEPTION(
        collection.set_entity_field(entity, "id", std::int64_t(999)),
        std::invalid_argument,
        "Setting 'id' field should throw exception"
    );

    // Test type mismatch
    TEST_EXCEPTION(
        collection.set_entity_field(entity, "name", std::int64_t(100)),
        std::invalid_argument,
        "Setting field with type mismatch should throw exception"
    );

    // Test nullable constraint
    TEST_EXCEPTION(
        collection.set_entity_field(entity, "age", NullType()),
        std::invalid_argument,
        "Setting non-nullable field to null should throw exception"
    );

    // Test nonexistent field
    TEST_EXCEPTION(
        collection.set_entity_field(entity, "nonexistent", std::string("value")),
        std::invalid_argument,
        "Setting nonexistent field should throw exception"
    );

    TEST_EXCEPTION(
        collection.get_entity_field(entity, "nonexistent"),
        std::invalid_argument,
        "Getting nonexistent field should throw exception"
    );

    std::cout << "  Entity field operations test passed" << std::endl;
}

// Test vector operations
void test_vector_operations()
{
    std::cout << "Testing vector operations..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));

    Collection collection("test", schema, 128, MetricType::L2);

    Entity & entity = collection.create_entity();

    // Test set_entity_vector
    std::vector<float> vec(128, 0.5f);
    collection.set_entity_vector(entity, "vector", vec);

    // Test get_entity_vector
    const auto & retrieved_vec = collection.get_entity_vector(entity, "vector");
    TEST_ASSERT(retrieved_vec.size() == 128, "Vector size should match");
    TEST_ASSERT(retrieved_vec[0] == 0.5f, "Vector value should match");

    // Test dimension mismatch
    std::vector<float> wrong_dim_vec(64, 0.5f);
    TEST_EXCEPTION(
        collection.set_entity_vector(entity, "vector", wrong_dim_vec),
        std::invalid_argument,
        "Setting vector with wrong dimension should throw exception"
    );

    // Test non-vector field
    TEST_EXCEPTION(
        collection.set_entity_vector(entity, "name", vec),
        std::invalid_argument,
        "Setting non-vector field as vector should throw exception"
    );

    TEST_EXCEPTION(
        collection.get_entity_vector(entity, "name"),
        std::invalid_argument,
        "Getting non-vector field as vector should throw exception"
    );

    std::cout << "  Vector operations test passed" << std::endl;
}

// Test ID mapping
void test_id_mapping()
{
    std::cout << "Testing ID mapping..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));

    Collection collection("test", schema, 128, MetricType::L2);

    // Create entities with string IDs
    Entity & entity1 = collection.create_entity("user_1");
    Entity & entity2 = collection.create_entity("user_2");

    std::int64_t id1 = collection.get_entity_id(entity1);
    std::int64_t id2 = collection.get_entity_id(entity2);

    // Test get_internal_id
    TEST_ASSERT(collection.get_internal_id("user_1") == id1, "Internal ID should match");
    TEST_ASSERT(collection.get_internal_id("user_2") == id2, "Internal ID should match");

    // Test get_string_id
    auto string_id1 = collection.get_string_id(id1);
    TEST_ASSERT(string_id1.has_value(), "String ID should exist");
    TEST_ASSERT(string_id1.value() == "user_1", "String ID should match");

    // Test get_entity_string_id
    TEST_ASSERT(collection.get_entity_string_id(entity1) == "user_1", "Entity string ID should match");

    // Test has_string_id
    TEST_ASSERT(collection.has_string_id("user_1") == true, "Should have string ID");
    TEST_ASSERT(collection.has_string_id("nonexistent") == false, "Should not have nonexistent string ID");

    // Test entity without string ID
    Entity & entity3 = collection.create_entity();  // auto ID, no string ID
    TEST_EXCEPTION(
        collection.get_entity_string_id(entity3),
        std::runtime_error,
        "Getting string ID for entity without string ID should throw exception"
    );

    // Test get_string_id for entity without string ID
    std::int64_t id3 = collection.get_entity_id(entity3);
    auto string_id3 = collection.get_string_id(id3);
    TEST_ASSERT(string_id3.has_value() == false, "Entity without string ID should return nullopt");

    // Test exceptions
    TEST_EXCEPTION(
        collection.get_internal_id("nonexistent"),
        std::invalid_argument,
        "get_internal_id with nonexistent string ID should throw exception"
    );

    std::cout << "  ID mapping test passed" << std::endl;
}

// Test entity retrieval
void test_entity_retrieval()
{
    std::cout << "Testing entity retrieval..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));

    Collection collection("test", schema, 128, MetricType::L2);

    // Create entities
    Entity & entity1 = collection.create_entity("user_1");
    collection.set_entity_field(entity1, "name", std::string("Alice"));

    Entity & entity2 = collection.create_entity();
    collection.set_entity_field(entity2, "name", std::string("Bob"));

    std::int64_t id1 = collection.get_entity_id(entity1);
    std::int64_t id2 = collection.get_entity_id(entity2);

    // Test get_entity by internal ID
    Entity & retrieved1 = collection.get_entity(id1);
    const auto & name1 = collection.get_entity_field(retrieved1, "name");
    TEST_ASSERT(std::get<std::string>(name1) == "Alice", "Retrieved entity name should match");

    // Test get_entity by string ID
    Entity & retrieved2 = collection.get_entity("user_1");
    const auto & name2 = collection.get_entity_field(retrieved2, "name");
    TEST_ASSERT(std::get<std::string>(name2) == "Alice", "Retrieved entity by string ID should match");

    // Test const versions
    const Collection & const_collection = collection;
    const Entity & const_entity1 = const_collection.get_entity(id1);
    const Entity & const_entity2 = const_collection.get_entity("user_1");

    // Test has_entity
    TEST_ASSERT(collection.has_entity(id1) == true, "Should have entity with ID 1");
    TEST_ASSERT(collection.has_entity(id2) == true, "Should have entity with ID 2");
    TEST_ASSERT(collection.has_entity(999) == false, "Should not have entity with ID 999");
    TEST_ASSERT(collection.has_entity("user_1") == true, "Should have entity with string ID");
    TEST_ASSERT(collection.has_entity("nonexistent") == false, "Should not have nonexistent entity");

    // Test exceptions
    TEST_EXCEPTION(
        collection.get_entity(999),
        std::invalid_argument,
        "get_entity with nonexistent ID should throw exception"
    );

    TEST_EXCEPTION(
        collection.get_entity("nonexistent"),
        std::invalid_argument,
        "get_entity with nonexistent string ID should throw exception"
    );

    std::cout << "  Entity retrieval test passed" << std::endl;
}

// Test entity deletion
void test_entity_deletion()
{
    std::cout << "Testing entity deletion..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));

    Collection collection("test", schema, 128, MetricType::L2);

    // Create entities
    Entity & entity1 = collection.create_entity("user_1");
    Entity & entity2 = collection.create_entity("user_2");
    Entity & entity3 = collection.create_entity();

    std::int64_t id1 = collection.get_entity_id(entity1);
    std::int64_t id2 = collection.get_entity_id(entity2);
    std::int64_t id3 = collection.get_entity_id(entity3);

    TEST_ASSERT(collection.entity_count() == 3, "Should have 3 entities");

    // Test delete_entity by internal ID
    collection.delete_entity(id1);
    TEST_ASSERT(collection.entity_count() == 2, "Should have 2 entities after deletion");
    TEST_ASSERT(collection.has_entity(id1) == false, "Entity 1 should not exist");
    TEST_ASSERT(collection.has_string_id("user_1") == false, "String ID mapping should be removed");

    // Test delete_entity by string ID
    collection.delete_entity("user_2");
    TEST_ASSERT(collection.entity_count() == 1, "Should have 1 entity after deletion");
    TEST_ASSERT(collection.has_entity(id2) == false, "Entity 2 should not exist");
    TEST_ASSERT(collection.has_string_id("user_2") == false, "String ID mapping should be removed");

    // Test exceptions
    TEST_EXCEPTION(
        collection.delete_entity(999),
        std::invalid_argument,
        "Deleting nonexistent entity should throw exception"
    );

    TEST_EXCEPTION(
        collection.delete_entity("nonexistent"),
        std::invalid_argument,
        "Deleting nonexistent entity by string ID should throw exception"
    );

    std::cout << "  Entity deletion test passed" << std::endl;
}

// Test default values
void test_default_values()
{
    std::cout << "Testing default values..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name", true, false, "", std::string("Unknown")));
    schema.push_back(Field::create_int64_field("age", true, false, "", std::int64_t(0)));

    Collection collection("test", schema, 128, MetricType::L2);

    Entity& entity = collection.create_entity();

    // Check default values
    const auto & name_value = collection.get_entity_field(entity, "name");
    TEST_ASSERT(std::holds_alternative<std::string>(name_value), "Name should have default value");
    TEST_ASSERT(std::get<std::string>(name_value) == "Unknown", "Name default value should match");

    const auto & age_value = collection.get_entity_field(entity, "age");
    TEST_ASSERT(std::holds_alternative<std::int64_t>(age_value), "Age should have default value");
    TEST_ASSERT(std::get<std::int64_t>(age_value) == 0, "Age default value should match");

    std::cout << "  Default values test passed" << std::endl;
}

// Test field_count and entity_count
void test_counts()
{
    std::cout << "Testing counts..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));
    schema.push_back(Field::create_int64_field("age"));
    schema.push_back(Field::create_boolean_field("active"));

    Collection collection("test", schema, 128, MetricType::L2);

    TEST_ASSERT(collection.field_count() == 5, "Should have 5 fields (id, vector, name, age, active)");
    TEST_ASSERT(collection.entity_count() == 0, "Should have 0 entities initially");

    collection.create_entity();
    collection.create_entity();
    collection.create_entity();

    TEST_ASSERT(collection.entity_count() == 3, "Should have 3 entities");

    std::cout << "  Counts test passed" << std::endl;
}

// Test duplicate field names in user schema
void test_duplicate_field_names()
{
    std::cout << "Testing duplicate field names..." << std::endl;

    std::vector<Field> schema;
    schema.push_back(Field::create_string_field("name"));
    schema.push_back(Field::create_string_field("name"));  // duplicate

    TEST_EXCEPTION(
        Collection("test", schema, 128, MetricType::L2),
        std::invalid_argument,
        "Collection with duplicate field names should throw exception"
    );

    std::cout << "  Duplicate field names test passed" << std::endl;
}

int main()
{
    std::cout << "Running Collection test cases..." << std::endl;
    std::cout << std::endl;

    try {
        test_constructor();
        test_constructor_validation();
        test_schema_access();
        test_field_access();
        test_entity_creation();
        test_entity_field_operations();
        test_vector_operations();
        test_id_mapping();
        test_entity_retrieval();
        test_entity_deletion();
        test_default_values();
        test_counts();
        test_duplicate_field_names();

        std::cout << std::endl;
        std::cout << "All tests passed!" << std::endl;
        return 0;
    }
    catch (const std::exception & e) {
        std::cerr << "Uncaught exception: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Uncaught unknown exception" << std::endl;
        return 1;
    }
}
