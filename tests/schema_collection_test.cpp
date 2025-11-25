#include <dreamdb/schema/collection.h>
#include <iostream>

#include "common.h"

using namespace dreamdb;

/**
 * @brief Minimal smoke test validating that the simplified Collection
 *        keeps its metadata intact.
 */
void test_basic_properties()
{
    std::vector<Field> schema;
    schema.push_back(Field::create_int64_field("id", true, true));
    schema.push_back(Field::create_varchar_field("name"));

    Collection collection("users", schema);

    TEST_ASSERT(collection.get_name() == "users", "Collection name should match");
    TEST_ASSERT(collection.get_schema().size() == 2, "Schema size should match");
}

int main()
{
    std::cout << "Running Collection smoke test..." << std::endl;
    std::cout << std::endl;

    try {
        test_basic_properties();

        std::cout << std::endl;
        std::cout << "Collection smoke test passed!" << std::endl;
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
