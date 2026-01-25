cmake_minimum_required(VERSION 3.14)
project(dreamdb VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 构建选项
option(DREAMDB_BUILD_TESTS "Build DreamDB tests" ON)
option(DREAMDB_BUILD_EXAMPLES "Build DreamDB examples/apps" OFF)

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

# 加载本地配置
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/local.cmake" OPTIONAL)

# ============================================================================
# Third Party Libraries
# ============================================================================

option(DREAMDB_BUILD_TESTS "Build DreamDB tests" ON)

include(cmake/deps/protobuf.cmake)
include(cmake/deps/asio.cmake)

if (DREAMDB_BUILD_TESTS)
  include(cmake/deps/googletest.cmake)
  enable_testing()
endif()

# ============================================================================
# Protobuf Code Generation
# ============================================================================

include(cmake/proto/proto.cmake)

set(PROTO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(PROTO_FILES
  	"${PROTO_ROOT}/dreamdb/protocol/message.proto"
)

# 生成到 build tree
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

dreamdb_protobuf_generate_cpp(
	PROTO_GEN_SRCS PROTO_GEN_HDRS
	PROTO_ROOT "${PROTO_ROOT}"
	OUT_DIR "${PROTO_GEN_DIR}"
	PROTOS ${PROTO_FILES}
)

# 确保生成的目录存在，以便 IDE 能够识别（即使在首次配置时文件还未生成）
file(MAKE_DIRECTORY "${PROTO_GEN_DIR}")

# ============================================================================
# DreamDB Library
# ============================================================================

# 添加全部头文件
file(GLOB_RECURSE DREAMDB_HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/dreamdb/*.inl"
)

# 添加指定部分源文件
file(GLOB_RECURSE DREAMDB_SOURCES
  	"${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/binder/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/planner/logical/*.cpp"
)

list(FILTER DREAMDB_SOURCES EXCLUDE REGEX ".*\\.pb\\.cc$")
list(FILTER DREAMDB_HEADERS EXCLUDE REGEX ".*\\.pb\\.h$")

add_library(dreamdb STATIC
	${DREAMDB_SOURCES}
	${DREAMDB_HEADERS}
	${PROTO_GEN_SRCS}
	${PROTO_GEN_HDRS}
)

# 添加 include 路径
# 注意：generated 目录必须作为 PUBLIC 包含路径，这样依赖 dreamdb 的目标也能找到生成的头文件
target_include_directories(dreamdb PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${PROTO_GEN_DIR}"
)

target_link_libraries(dreamdb PUBLIC
	dreamdb::protobuf
	dreamdb::asio
)

# ============================================================================
# Examples
# ============================================================================

if (DREAMDB_BUILD_EXAMPLES)
    add_executable(server_demo "${CMAKE_CURRENT_SOURCE_DIR}/examples/server_demo.cpp")
    target_link_libraries(server_demo PRIVATE dreamdb)

    add_executable(client_demo "${CMAKE_CURRENT_SOURCE_DIR}/examples/client_demo.cpp")
    target_link_libraries(client_demo PRIVATE dreamdb)

    add_executable(auto_client "${CMAKE_CURRENT_SOURCE_DIR}/examples/auto_client.cpp")
    target_link_libraries(auto_client PRIVATE dreamdb)

    add_custom_command(TARGET auto_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/examples/test.sql"
            "$<TARGET_FILE_DIR:auto_client>/test.sql"
    COMMENT "Copying test.sql next to auto_client"
    )
endif()

# ============================================================================
# Tests
# ============================================================================

# 添加测试目录
add_subdirectory(tests)
