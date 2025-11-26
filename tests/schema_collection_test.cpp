#include <dreamdb/schema/collection.h>
#include <dreamdb/storage/segment_manager.h>
#include <dreamdb/storage/memory_storage.h>
#include <iostream>
#include <memory>

#include "common.h"

using namespace dreamdb;

SegmentManager make_segment_manager()
{
    return SegmentManager([](std::size_t field_count) {
        return std::make_unique<MemoryStorage>(field_count);
    });
}

void test_basic_properties()
{
    auto manager = make_segment_manager();

    std::vector<Field> schema;
    schema.push_back(Field::create_int64_field("id", true, true));
    schema.push_back(Field::create_varchar_field("name"));

    Collection collection("users", schema, manager);

    TEST_ASSERT(collection.get_name() == "users", "Collection name should match");
    TEST_ASSERT(collection.get_schema().size() == 2, "Schema size should match");
}

void test_insert_flow()
{
    auto manager = make_segment_manager();

    std::vector<Field> schema;
    schema.push_back(Field::create_int64_field("age"));
    schema.push_back(Field::create_varchar_field("name"));

    Collection collection("people", schema, manager);

    Entity entity = collection.create_entity();
    entity.set_value(0, static_cast<std::int64_t>(32));
    entity.set_value(1, std::string("Alice"));

    MutationResult result = collection.insert(entity);
    TEST_ASSERT(result.success, "Insert operation should succeed");

    auto segments = manager.get_segments("people");
    TEST_ASSERT(!segments.empty(), "Segment should be created after insert");

    auto * storage = segments.front()->get_storage();
    TEST_ASSERT(storage != nullptr, "Segment storage should exist");

    auto stored = storage->get_by_id(entity.get_id());
    TEST_ASSERT(stored != nullptr, "Stored entity should be retrievable");
    TEST_ASSERT(std::get<std::int64_t>(stored->get_value(0)) == 32, "Age should match");
    TEST_ASSERT(std::get<std::string>(stored->get_value(1)) == "Alice", "Name should match");
}

int main()
{
    std::cout << "Running Collection tests..." << std::endl;
    std::cout << std::endl;

    try {
        test_basic_properties();
        test_insert_flow();

        std::cout << std::endl;
        std::cout << "All Collection tests passed!" << std::endl;
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
