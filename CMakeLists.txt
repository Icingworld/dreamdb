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
    set(PLATFORM_LIBS ws2_32 wsock32)
elseif(UNIX)
    set(PLATFORM_LIBS pthread)
endif()

# 添加 protobuf 库
set(Protobuf_ROOT "D:/protobuf")
set(CMAKE_PREFIX_PATH ${Protobuf_ROOT} ${CMAKE_PREFIX_PATH})

find_package(Protobuf REQUIRED)

# 添加第三方库
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/include"
)

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

# 链接平台库
target_link_libraries(dreamdb PUBLIC ${PLATFORM_LIBS} protobuf::libprotobuf)

# 测试支持
enable_testing()
