cmake_minimum_required(VERSION 3.14)
project(dreamdb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE DREAMDB_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.inl"
)

file(GLOB_RECURSE DREAMDB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(dreamdb STATIC
    ${DREAMDB_SOURCES}
    ${DREAMDB_HEADERS}
)

target_include_directories(dreamdb PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 编译测试用例
add_executable(schema_field_test tests/schema_field_test.cpp)
target_include_directories(schema_field_test PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
)
target_link_libraries(schema_field_test dreamdb)
