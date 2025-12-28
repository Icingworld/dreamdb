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
# 可以通过 -DProtobuf_ROOT=<path> 来指定 protobuf 路径
if(NOT DEFINED Protobuf_ROOT)
    if(WIN32)
        set(Protobuf_ROOT "C:/msys64/ucrt64")
    endif()
endif()

if(Protobuf_ROOT)
    set(CMAKE_PREFIX_PATH ${Protobuf_ROOT} ${CMAKE_PREFIX_PATH})
endif()

find_package(Protobuf REQUIRED)

# 找到 protoc 编译器
find_program(PROTOC_EXECUTABLE protoc 
    HINTS ${Protobuf_ROOT}/bin
    REQUIRED
)
message(STATUS "Found protoc: ${PROTOC_EXECUTABLE}")

# Protobuf 依赖的 absl 和 utf8_range 库（手动链接）
if(NOT DEFINED PROTOBUF_LIB_DIR)
    if(Protobuf_ROOT)
        set(PROTOBUF_LIB_DIR "${Protobuf_ROOT}/lib")
    else()
        set(PROTOBUF_LIB_DIR "/usr/lib")
    endif()
endif()

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
# Protobuf Code Generation
# ============================================================================

# Proto 文件路径
set(PROTO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/protocol")
set(PROTO_FILES "${PROTO_DIR}/message.proto")

# 生成文件输出目录（单独的 proto 目录）
set(PROTO_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto/dreamdb/protocol")

# 确保输出目录存在
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

# 定义生成的文件
set(PROTO_GEN_SRCS "${PROTO_GEN_DIR}/message.pb.cc")
set(PROTO_GEN_HDRS "${PROTO_GEN_DIR}/message.pb.h")

# 配置期生成 protobuf 文件
if(NOT EXISTS ${PROTO_GEN_SRCS} OR 
   NOT EXISTS ${PROTO_GEN_HDRS} OR
   ${PROTO_FILES} IS_NEWER_THAN ${PROTO_GEN_SRCS})
    
    message(STATUS "Generating protobuf files from message.proto...")
    execute_process(
        COMMAND ${PROTOC_EXECUTABLE}
            --proto_path=${PROTO_DIR}
            --cpp_out=${PROTO_GEN_DIR}
            ${PROTO_FILES}
        RESULT_VARIABLE PROTO_RESULT
    )
    if(NOT PROTO_RESULT EQUAL 0)
        message(FATAL_ERROR "protoc failed!")
    endif()
endif()

# 自定义命令：生成 protobuf 代码
add_custom_command(
    OUTPUT ${PROTO_GEN_SRCS} ${PROTO_GEN_HDRS}
    COMMAND ${PROTOC_EXECUTABLE}
        --proto_path=${PROTO_DIR}
        --cpp_out=${PROTO_GEN_DIR}
        ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
    COMMENT "Generating protobuf files from message.proto..."
    VERBATIM
)

# 创建一个目标来触发 protobuf 生成
add_custom_target(proto_gen DEPENDS ${PROTO_GEN_SRCS} ${PROTO_GEN_HDRS})

# ============================================================================
# DreamDB Library
# ============================================================================

# 收集源文件（排除自动生成的 protobuf 文件，因为它们会单独添加）
file(GLOB_RECURSE DREAMDB_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/**/*.inl"
)

file(GLOB_RECURSE DREAMDB_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cc"
)

# 排除自动生成的 protobuf 文件
list(FILTER DREAMDB_SOURCES EXCLUDE REGEX ".*\\.pb\\.cc$")
list(FILTER DREAMDB_HEADERS EXCLUDE REGEX ".*\\.pb\\.h$")

# 创建主库（包含自动生成的 protobuf 文件）
add_library(dreamdb STATIC
    ${DREAMDB_SOURCES}
    ${DREAMDB_HEADERS}
    ${PROTO_GEN_SRCS}
    ${PROTO_GEN_HDRS}
)

# 确保先生成 protobuf 文件
add_dependencies(dreamdb proto_gen)

target_include_directories(dreamdb PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/proto"
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

# Auto Client
add_executable(auto_client
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/auto_client.cpp"
)
target_link_libraries(auto_client PRIVATE dreamdb)

# 复制 test.sql 到 build 目录
add_custom_command(TARGET auto_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/examples/test.sql"
    "${CMAKE_CURRENT_BINARY_DIR}/test.sql"
    COMMENT "Copying test.sql to build directory"
)