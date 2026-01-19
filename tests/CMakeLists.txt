# ============================================================================
# Tests
# ============================================================================

if(NOT DREAMDB_BUILD_TESTS)
    return()
endif()

# 添加测试目录
add_executable(unit_parser_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/parser/statement/use_test.cpp
)

target_link_libraries(unit_parser_tests PRIVATE
    dreamdb
    gtest
    gtest_main
)

add_test(NAME unit_parser_tests COMMAND unit_parser_tests)
