cmake_minimum_required(VERSION 3.14)
project(dreamdb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_WIN32_WINNT=0x0601)  # Windows 7+
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 平台特定的链接库
if(WIN32)
    set(PLATFORM_LIBS ws2_32 wsock32 dbghelp bcrypt)
elseif(UNIX)
    set(PLATFORM_LIBS pthread)
endif()

# ============================================================================
# Third Party Libraries
# ============================================================================

# 添加 protobuf 库
set(Protobuf_ROOT "D:/protobuf")
set(CMAKE_PREFIX_PATH ${Protobuf_ROOT} ${CMAKE_PREFIX_PATH})

find_package(Protobuf REQUIRED)

# Protobuf 依赖的 absl 和 utf8_range 库（手动链接）
set(PROTOBUF_LIB_DIR "${Protobuf_ROOT}/lib")

# 收集所有 absl 库（使用通配符，然后多次链接以解决循环依赖）
file(GLOB ABSL_ALL_LIBS "${PROTOBUF_LIB_DIR}/libabsl_*.a")

# utf8_range 库
set(UTF8_RANGE_LIBS
    ${PROTOBUF_LIB_DIR}/libutf8_range.a
    ${PROTOBUF_LIB_DIR}/libutf8_validity.a
)

# 添加其他第三方库
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/include"
)

# ============================================================================
# DreamDB Library
# ============================================================================

# 收集源文件
file(GLOB_RECURSE DREAMDB_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.inl"
)

file(GLOB_RECURSE DREAMDB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cc"
)

# 创建主库
add_library(dreamdb STATIC
    ${DREAMDB_SOURCES}
    ${DREAMDB_HEADERS}
)

target_include_directories(dreamdb PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# 链接平台库和 Protobuf 依赖
# 注意：absl 库之间有复杂的循环依赖，需要多次链接或使用 --start-group/--end-group
# dbghelp 需要放在 absl 之后（因为 absl_symbolize 依赖它）
target_link_libraries(dreamdb PUBLIC 
    protobuf::libprotobuf
    ${UTF8_RANGE_LIBS}
    -Wl,--start-group
    ${ABSL_ALL_LIBS}
    -Wl,--end-group
    ${PLATFORM_LIBS}
)

# 测试支持
enable_testing()

# ============================================================================
# Examples
# ============================================================================

# Server Demo
add_executable(server_demo
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/server_demo.cpp"
)
target_link_libraries(server_demo PRIVATE dreamdb)

# Client Demo
add_executable(client_demo
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/client_demo.cpp"
)
target_link_libraries(client_demo PRIVATE dreamdb)
