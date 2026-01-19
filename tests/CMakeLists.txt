# ============================================================================
# Tests
# ============================================================================

if(NOT DREAMDB_BUILD_TESTS)
    return()
endif()

# 添加测试目录
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
    gtest
    gtest_main
)

add_test(NAME unit_parser_tests COMMAND unit_parser_tests)
