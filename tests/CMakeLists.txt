# ============================================================================
# Tests
# ============================================================================

if(NOT DREAMDB_BUILD_TESTS)
    return()
endif()

# 添加 parser 测试目录
add_executable(unit_parser_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/use_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/describe_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/drop_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/show_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/delete_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/update_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/insert_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/create_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/alter_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/select_test.cpp
)

target_link_libraries(unit_parser_tests PRIVATE
    dreamdb
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME unit_parser_tests COMMAND unit_parser_tests)

# 添加 binder 测试目录
add_executable(unit_binder_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/use_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/describe_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/show_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/drop_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/delete_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/update_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/insert_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/select_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/alter_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/binder/create_test.cpp
)

target_link_libraries(unit_binder_tests PRIVATE
    dreamdb
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

add_test(NAME unit_binder_tests COMMAND unit_binder_tests)
