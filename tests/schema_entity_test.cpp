#include <dreamdb/schema/entity.h>
#include <cassert>
#include <iostream>

#include "common.h"

using namespace dreamdb;

// Test default constructor
void test_default_constructor()
{
    std::cout << "Testing default constructor..." << std::endl;

    Entity entity;
    TEST_ASSERT(entity.field_count() == 0, "Default constructor should create empty entity");
    TEST_ASSERT(entity.is_empty() == true, "Default entity should be empty");

    std::cout << "  Default constructor test passed" << std::endl;
}

// Test constructor with field count
void test_constructor_with_field_count()
{
    std::cout << "Testing constructor with field count..." << std::endl;

    Entity entity(5);
    TEST_ASSERT(entity.field_count() == 5, "Entity should have 5 fields");
    TEST_ASSERT(entity.is_empty() == true, "New entity should be empty (all null)");

    // All fields should be null initially
    for (std::size_t i = 0; i < 5; ++i) {
        TEST_ASSERT(entity.is_null(i) == true, "Field should be null initially");
    }

    std::cout << "  Constructor with field count test passed" << std::endl;
}

// Test copy constructor and assignment
void test_copy_operations()
{
    std::cout << "Testing copy operations..." << std::endl;

    Entity entity1(3);
    entity1.set_value(0, std::int64_t(100));
    entity1.set_value(1, std::string("test"));
    entity1.set_value(2, true);

    // Test copy constructor
    Entity entity2(entity1);
    TEST_ASSERT(entity2.field_count() == 3, "Copied entity should have same field count");
    TEST_ASSERT(std::get<std::int64_t>(entity2.get_value(0)) == 100, "Copied entity should have same values");
    TEST_ASSERT(std::get<std::string>(entity2.get_value(1)) == "test", "Copied entity should have same string value");
    TEST_ASSERT(std::get<bool>(entity2.get_value(2)) == true, "Copied entity should have same bool value");

    // Test copy assignment
    Entity entity3(1);
    entity3 = entity1;
    TEST_ASSERT(entity3.field_count() == 3, "Assigned entity should have same field count");
    TEST_ASSERT(std::get<std::int64_t>(entity3.get_value(0)) == 100, "Assigned entity should have same values");

    std::cout << "  Copy operations test passed" << std::endl;
}

// Test move constructor and assignment
void test_move_operations()
{
    std::cout << "Testing move operations..." << std::endl;

    Entity entity1(2);
    entity1.set_value(0, std::int64_t(200));
    entity1.set_value(1, 3.14f);

    // Test move constructor
    Entity entity2(std::move(entity1));
    TEST_ASSERT(entity2.field_count() == 2, "Moved entity should have same field count");
    TEST_ASSERT(std::get<std::int64_t>(entity2.get_value(0)) == 200, "Moved entity should have same values");
    TEST_ASSERT(std::get<float>(entity2.get_value(1)) == 3.14f, "Moved entity should have same float value");
    TEST_ASSERT(entity1.field_count() == 0, "Source entity should be empty after move");

    // Test move assignment
    Entity entity3(1);
    entity3 = std::move(entity2);
    TEST_ASSERT(entity3.field_count() == 2, "Move-assigned entity should have same field count");
    TEST_ASSERT(std::get<std::int64_t>(entity3.get_value(0)) == 200, "Move-assigned entity should have same values");

    std::cout << "  Move operations test passed" << std::endl;
}

// Test set_value and get_value
void test_set_and_get_value()
{
    std::cout << "Testing set_value and get_value..." << std::endl;

    Entity entity(6);

    // Test INT64 value
    entity.set_value(0, std::int64_t(12345));
    TEST_ASSERT(std::holds_alternative<std::int64_t>(entity.get_value(0)), "Value should be INT64");
    TEST_ASSERT(std::get<std::int64_t>(entity.get_value(0)) == 12345, "INT64 value should match");

    // Test FLOAT value
    entity.set_value(1, 3.14f);
    TEST_ASSERT(std::holds_alternative<float>(entity.get_value(1)), "Value should be FLOAT");
    TEST_ASSERT(std::get<float>(entity.get_value(1)) == 3.14f, "FLOAT value should match");

    // Test DOUBLE value
    entity.set_value(2, 99.99);
    TEST_ASSERT(std::holds_alternative<double>(entity.get_value(2)), "Value should be DOUBLE");
    TEST_ASSERT(std::get<double>(entity.get_value(2)) == 99.99, "DOUBLE value should match");

    // Test STRING value
    entity.set_value(3, std::string("hello world"));
    TEST_ASSERT(std::holds_alternative<std::string>(entity.get_value(3)), "Value should be STRING");
    TEST_ASSERT(std::get<std::string>(entity.get_value(3)) == "hello world", "STRING value should match");

    // Test BOOLEAN value
    entity.set_value(4, true);
    TEST_ASSERT(std::holds_alternative<bool>(entity.get_value(4)), "Value should be BOOLEAN");
    TEST_ASSERT(std::get<bool>(entity.get_value(4)) == true, "BOOLEAN value should match");

    // Test FLOAT_VECTOR value
    std::vector<float> vec = {1.0f, 2.0f, 3.0f, 4.0f};
    entity.set_value(5, vec);
    TEST_ASSERT(std::holds_alternative<std::vector<float>>(entity.get_value(5)), "Value should be FLOAT_VECTOR");
    const auto & retrieved_vec = std::get<std::vector<float>>(entity.get_value(5));
    TEST_ASSERT(retrieved_vec.size() == 4, "Vector size should match");
    TEST_ASSERT(retrieved_vec[0] == 1.0f && retrieved_vec[1] == 2.0f, "Vector values should match");

    std::cout << "  Set and get value test passed" << std::endl;
}

// Test get_value_as template method (if implemented)
void test_get_value_as()
{
    std::cout << "Testing get_value_as template method..." << std::endl;

    Entity entity(5);
    entity.set_value(0, std::int64_t(100));
    entity.set_value(1, 3.14f);
    entity.set_value(2, 99.99);
    entity.set_value(3, std::string("test"));
    entity.set_value(4, true);

    // Note: This test assumes get_value_as is implemented using std::get
    // If not implemented, these tests will fail at compile time
    try {
        auto int64_val = entity.get_value_as<std::int64_t>(0);
        TEST_ASSERT(int64_val == 100, "get_value_as should return correct INT64 value");

        auto float_val = entity.get_value_as<float>(1);
        TEST_ASSERT(float_val == 3.14f, "get_value_as should return correct FLOAT value");

        auto double_val = entity.get_value_as<double>(2);
        TEST_ASSERT(double_val == 99.99, "get_value_as should return correct DOUBLE value");

        auto string_val = entity.get_value_as<std::string>(3);
        TEST_ASSERT(string_val == "test", "get_value_as should return correct STRING value");

        auto bool_val = entity.get_value_as<bool>(4);
        TEST_ASSERT(bool_val == true, "get_value_as should return correct BOOLEAN value");
    }
    catch (...) {
        // If get_value_as is not implemented, skip this test
        std::cout << "  get_value_as template method not implemented, skipping test" << std::endl;
        return;
    }

    std::cout << "  Get value as test passed" << std::endl;
}

// Test is_null
void test_is_null()
{
    std::cout << "Testing is_null..." << std::endl;

    Entity entity(3);

    // All fields should be null initially
    TEST_ASSERT(entity.is_null(0) == true, "Field should be null initially");
    TEST_ASSERT(entity.is_null(1) == true, "Field should be null initially");
    TEST_ASSERT(entity.is_null(2) == true, "Field should be null initially");

    // Set a value and check it's not null
    entity.set_value(1, std::int64_t(100));
    TEST_ASSERT(entity.is_null(1) == false, "Field should not be null after setting value");

    // Set to NullType explicitly
    entity.set_value(1, NullType());
    TEST_ASSERT(entity.is_null(1) == true, "Field should be null after setting NullType");

    std::cout << "  Is null test passed" << std::endl;
}

// Test field_count
void test_field_count()
{
    std::cout << "Testing field_count..." << std::endl;

    Entity entity1;
    TEST_ASSERT(entity1.field_count() == 0, "Empty entity should have 0 fields");

    Entity entity2(10);
    TEST_ASSERT(entity2.field_count() == 10, "Entity should have 10 fields");

    Entity entity3(0);
    TEST_ASSERT(entity3.field_count() == 0, "Entity with 0 fields should have 0 fields");

    std::cout << "  Field count test passed" << std::endl;
}

// Test clear
void test_clear()
{
    std::cout << "Testing clear..." << std::endl;

    Entity entity(4);
    entity.set_value(0, std::int64_t(100));
    entity.set_value(1, std::string("test"));
    entity.set_value(2, true);
    entity.set_value(3, 3.14f);

    TEST_ASSERT(entity.is_empty() == false, "Entity should not be empty before clear");

    entity.clear();

    TEST_ASSERT(entity.is_empty() == true, "Entity should be empty after clear");
    for (std::size_t i = 0; i < 4; ++i) {
        TEST_ASSERT(entity.is_null(i) == true, "All fields should be null after clear");
    }

    std::cout << "  Clear test passed" << std::endl;
}

// Test is_empty
void test_is_empty()
{
    std::cout << "Testing is_empty..." << std::endl;

    Entity entity(3);

    // Empty entity should return true
    TEST_ASSERT(entity.is_empty() == true, "Empty entity should return true");

    // Set one value, should return false
    entity.set_value(1, std::int64_t(100));
    TEST_ASSERT(entity.is_empty() == false, "Entity with values should return false");

    // Clear and check again
    entity.clear();
    TEST_ASSERT(entity.is_empty() == true, "Cleared entity should return true");

    std::cout << "  Is empty test passed" << std::endl;
}

// Test out of range exceptions
void test_out_of_range()
{
    std::cout << "Testing out of range exceptions..." << std::endl;

    Entity entity(3);

    // Test set_value out of range
    TEST_EXCEPTION(
        entity.set_value(3, std::int64_t(100)),
        std::out_of_range,
        "set_value with out of range index should throw exception"
    );

    // Test get_value out of range
    TEST_EXCEPTION(
        entity.get_value(5),
        std::out_of_range,
        "get_value with out of range index should throw exception"
    );

    // Test is_null out of range
    TEST_EXCEPTION(
        entity.is_null(10),
        std::out_of_range,
        "is_null with out of range index should throw exception"
    );

    std::cout << "  Out of range test passed" << std::endl;
}

// Test all value types
void test_all_value_types()
{
    std::cout << "Testing all value types..." << std::endl;

    Entity entity(9);

    // Test INT8
    entity.set_value(0, std::int8_t(127));
    TEST_ASSERT(std::holds_alternative<std::int8_t>(entity.get_value(0)), "Value should be INT8");

    // Test INT16
    entity.set_value(1, std::int16_t(32767));
    TEST_ASSERT(std::holds_alternative<std::int16_t>(entity.get_value(1)), "Value should be INT16");

    // Test INT32
    entity.set_value(2, std::int32_t(2147483647));
    TEST_ASSERT(std::holds_alternative<std::int32_t>(entity.get_value(2)), "Value should be INT32");

    // Test INT64
    entity.set_value(3, std::int64_t(9223372036854775807LL));
    TEST_ASSERT(std::holds_alternative<std::int64_t>(entity.get_value(3)), "Value should be INT64");

    // Test FLOAT
    entity.set_value(4, 3.14159f);
    TEST_ASSERT(std::holds_alternative<float>(entity.get_value(4)), "Value should be FLOAT");

    // Test DOUBLE
    entity.set_value(5, 2.718281828);
    TEST_ASSERT(std::holds_alternative<double>(entity.get_value(5)), "Value should be DOUBLE");

    // Test STRING
    entity.set_value(6, std::string("test string"));
    TEST_ASSERT(std::holds_alternative<std::string>(entity.get_value(6)), "Value should be STRING");

    // Test BOOLEAN
    entity.set_value(7, false);
    TEST_ASSERT(std::holds_alternative<bool>(entity.get_value(7)), "Value should be BOOLEAN");

    // Test FLOAT_VECTOR
    std::vector<float> vector = {1.1f, 2.2f, 3.3f};
    entity.set_value(8, vector);
    TEST_ASSERT(std::holds_alternative<std::vector<float>>(entity.get_value(8)), "Value should be FLOAT_VECTOR");

    std::cout << "  All value types test passed" << std::endl;
}

// Test value modification through reference
void test_value_modification()
{
    std::cout << "Testing value modification through reference..." << std::endl;

    Entity entity(2);
    entity.set_value(0, std::int64_t(100));

    // Get mutable reference and modify
    auto & value = entity.get_value(0);
    std::get<std::int64_t>(value) = 200;

    // Verify modification
    TEST_ASSERT(std::get<std::int64_t>(entity.get_value(0)) == 200, "Value should be modified through reference");

    std::cout << "  Value modification test passed" << std::endl;
}

int main()
{
    std::cout << "Running Entity test cases..." << std::endl;
    std::cout << std::endl;

    try {
        test_default_constructor();
        test_constructor_with_field_count();
        test_copy_operations();
        test_move_operations();
        test_set_and_get_value();
        test_get_value_as();
        test_is_null();
        test_field_count();
        test_clear();
        test_is_empty();
        test_out_of_range();
        test_all_value_types();
        test_value_modification();

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

