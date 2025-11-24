#include <dreamdb/schema/field.h>
#include <cassert>
#include <iostream>

#include "common.h"

using namespace dreamdb;

// Test field creation for all field types
void test_field_creation()
{
    std::cout << "Testing field creation..." << std::endl;

    // Test INT64 field
    Field int64_field = Field::create_int64_field("id", false, true, "Primary Key ID");
    TEST_ASSERT(int64_field.get_name() == "id", "INT64 field name");
    TEST_ASSERT(int64_field.get_type() == FieldType::INT64, "INT64 field type");
    TEST_ASSERT(int64_field.get_is_nullable() == false, "INT64 field nullable");
    TEST_ASSERT(int64_field.get_is_primary() == true, "INT64 field primary key");
    TEST_ASSERT(int64_field.get_comment() == "Primary Key ID", "INT64 field comment");

    // Test FLOAT field
    Field float_field = Field::create_float_field("price");
    TEST_ASSERT(float_field.get_type() == FieldType::FLOAT, "FLOAT field type");
    TEST_ASSERT(float_field.get_is_nullable() == true, "FLOAT field default nullable");

    // Test DOUBLE field
    Field double_field = Field::create_double_field("amount");
    TEST_ASSERT(double_field.get_type() == FieldType::DOUBLE, "DOUBLE field type");

    // Test CHAR field
    Field char_field = Field::create_char_field("code", true, false, "", NullType());
    TEST_ASSERT(char_field.get_type() == FieldType::CHAR, "CHAR field type");

    // Test VARCHAR field
    Field varchar_field = Field::create_varchar_field("title");
    TEST_ASSERT(varchar_field.get_type() == FieldType::VARCHAR, "VARCHAR field type");

    // Test STRING field
    Field string_field = Field::create_varchar_field("name");
    TEST_ASSERT(string_field.get_type() == FieldType::VARCHAR, "VARCHAR field (string alias) type");
    TEST_ASSERT(string_field.get_name() == "name", "VARCHAR field name");

    // Test BOOLEAN field
    Field bool_field = Field::create_boolean_field("is_active");
    TEST_ASSERT(bool_field.get_type() == FieldType::BOOLEAN, "BOOLEAN field type");

    // Test TIMESTAMP field
    Field timestamp_field = Field::create_timestamp_field("created_at");
    TEST_ASSERT(timestamp_field.get_type() == FieldType::TIMESTAMP, "TIMESTAMP field type");

    // Test ENUM field
    Field enum_field = Field::create_enum_field("status");
    TEST_ASSERT(enum_field.get_type() == FieldType::ENUM, "ENUM field type");

    // Test FLOAT_VECTOR field
    Field vector_field = Field::create_float_vector_field("embedding", true, false, "", NullType());
    TEST_ASSERT(vector_field.get_type() == FieldType::FLOAT_VECTOR, "FLOAT_VECTOR field type");

    std::cout << "  Field creation test passed" << std::endl;
}

// Test getter and setter methods
void test_getters_and_setters()
{
    std::cout << "Testing getter and setter methods..." << std::endl;

    Field field = Field::create_varchar_field("test_field");

    // Test name setter and getter
    field.set_name("new_name");
    TEST_ASSERT(field.get_name() == "new_name", "Field name setter and getter");

    // Test type setter and getter
    field.set_type(FieldType::INT64);
    TEST_ASSERT(field.get_type() == FieldType::INT64, "Field type setter and getter");

    // Test length setter and getter
    field.set_length(100);
    TEST_ASSERT(field.get_length() == 100, "Field length setter and getter");

    // Test precision setter and getter
    field.set_precision(2);
    TEST_ASSERT(field.get_precision() == 2, "Field precision setter and getter");

    // Test nullable setter and getter
    field.set_is_nullable(false);
    TEST_ASSERT(field.get_is_nullable() == false, "Field nullable setter and getter");

    // Test primary key setter and getter
    field.set_is_primary(true);
    TEST_ASSERT(field.get_is_primary() == true, "Field primary key setter and getter");

    // Test comment setter and getter
    field.set_comment("Test comment");
    TEST_ASSERT(field.get_comment() == "Test comment", "Field comment setter and getter");

    std::cout << "  Getter and setter test passed" << std::endl;
}

// Test default value setting
void test_default_values()
{
    std::cout << "Testing default value setting..." << std::endl;

    // Test INT64 field default value
    Field int64_field = Field::create_int64_field("id");
    int64_field.set_default_value(std::int64_t(100));
    const auto & int64_default = int64_field.get_default_value();
    TEST_ASSERT(std::holds_alternative<std::int64_t>(int64_default), "INT64 default value type");
    TEST_ASSERT(std::get<std::int64_t>(int64_default) == 100, "INT64 default value content");

    // Test FLOAT field default value
    Field float_field = Field::create_float_field("price");
    float_field.set_default_value(3.14f);
    const auto & float_default = float_field.get_default_value();
    TEST_ASSERT(std::holds_alternative<float>(float_default), "FLOAT default value type");
    TEST_ASSERT(std::get<float>(float_default) == 3.14f, "FLOAT default value content");

    // Test DOUBLE field default value
    Field double_field = Field::create_double_field("amount");
    double_field.set_default_value(99.99);
    const auto & double_default = double_field.get_default_value();
    TEST_ASSERT(std::holds_alternative<double>(double_default), "DOUBLE default value type");
    TEST_ASSERT(std::get<double>(double_default) == 99.99, "DOUBLE default value content");

    // Test STRING field default value
    Field string_field = Field::create_varchar_field("name");
    string_field.set_default_value(std::string("default_name"));
    const auto & string_default = string_field.get_default_value();
    TEST_ASSERT(std::holds_alternative<std::string>(string_default), "STRING default value type");
    TEST_ASSERT(std::get<std::string>(string_default) == "default_name", "STRING default value content");

    // Test BOOLEAN field default value
    Field bool_field = Field::create_boolean_field("is_active");
    bool_field.set_default_value(true);
    const auto & bool_default = bool_field.get_default_value();
    TEST_ASSERT(std::holds_alternative<bool>(bool_default), "BOOLEAN default value type");
    TEST_ASSERT(std::get<bool>(bool_default) == true, "BOOLEAN default value content");

    // Test NULL default value
    Field null_field = Field::create_varchar_field("optional");
    null_field.set_default_value(NullType());
    const auto & null_default = null_field.get_default_value();
    TEST_ASSERT(std::holds_alternative<NullType>(null_default), "NULL default value type");

    std::cout << "  Default value setting test passed" << std::endl;
}

// Test default value type validation
void test_default_value_validation()
{
    std::cout << "Testing default value type validation..." << std::endl;

    // Test type mismatch default value (should throw exception)
    Field int64_field = Field::create_int64_field("id");
    TEST_EXCEPTION(
        int64_field.set_default_value(std::string("invalid")),
        std::invalid_argument,
        "INT64 field setting string default value should throw exception"
    );

    Field string_field = Field::create_varchar_field("name");
    TEST_EXCEPTION(
        string_field.set_default_value(100),
        std::invalid_argument,
        "STRING field setting integer default value should throw exception"
    );

    // Test vector field does not support default value
    Field vector_field = Field::create_float_vector_field("embedding", true, false, "", NullType());
    TEST_EXCEPTION(
        vector_field.set_default_value(1.0f),
        std::invalid_argument,
        "Vector field setting default value should throw exception"
    );

    // Test enum field does not support default value
    Field enum_field = Field::create_enum_field("status");
    TEST_EXCEPTION(
        enum_field.set_default_value(std::string("active")),
        std::invalid_argument,
        "Enum field setting default value should throw exception"
    );

    std::cout << "  Default value type validation test passed" << std::endl;
}

// Test auto increment field
void test_auto_increment()
{
    std::cout << "Testing auto increment field..." << std::endl;

    // Test INT64 field can set auto increment
    Field int64_field = Field::create_int64_field("id");
    int64_field.set_is_auto_increment(true);
    TEST_ASSERT(int64_field.get_is_auto_increment() == true, "INT64 field auto increment setting");

    // Test non-INT64 field cannot set auto increment (should throw exception)
    Field string_field = Field::create_varchar_field("name");
    TEST_EXCEPTION(
        string_field.set_is_auto_increment(true),
        std::invalid_argument,
        "Non-INT64 field setting auto increment should throw exception"
    );

    std::cout << "  Auto increment field test passed" << std::endl;
}

// Test edge cases
void test_edge_cases()
{
    std::cout << "Testing edge cases..." << std::endl;

    // Test empty name
    Field empty_name_field = Field::create_varchar_field("");
    TEST_ASSERT(empty_name_field.get_name() == "", "Empty name field");

    // Test empty comment
    Field empty_comment_field = Field::create_varchar_field("field", true, false, "");
    TEST_ASSERT(empty_comment_field.get_comment() == "", "Empty comment field");

    // Test all field type creation (no parameters)
    Field int64_default = Field::create_int64_field();
    TEST_ASSERT(int64_default.get_type() == FieldType::INT64, "Default INT64 field");

    Field float_default = Field::create_float_field();
    TEST_ASSERT(float_default.get_type() == FieldType::FLOAT, "Default FLOAT field");

    Field double_default = Field::create_double_field();
    TEST_ASSERT(double_default.get_type() == FieldType::DOUBLE, "Default DOUBLE field");

    Field string_default = Field::create_varchar_field();
    TEST_ASSERT(string_default.get_type() == FieldType::VARCHAR, "Default VARCHAR field");

    Field bool_default = Field::create_boolean_field();
    TEST_ASSERT(bool_default.get_type() == FieldType::BOOLEAN, "Default BOOLEAN field");

    std::cout << "  Edge cases test passed" << std::endl;
}

int main()
{
    std::cout << "Running Field test cases..." << std::endl;
    std::cout << std::endl;

    try {
        test_field_creation();
        test_getters_and_setters();
        test_default_values();
        test_default_value_validation();
        test_auto_increment();
        test_edge_cases();

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
