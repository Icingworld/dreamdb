# ============================================================================
#  Protobuf
# ============================================================================

if (TARGET dreamdb::protobuf AND TARGET dreamdb::protoc)
    return()
endif()

# 允许用户 -DProtobuf_ROOT=... 传入
if (NOT DEFINED Protobuf_ROOT AND WIN32)
    message(FATAL_ERROR "Protobuf_ROOT is not set, please set it in cmake/local.cmake")
endif()

# 如果指定了 Protobuf_ROOT，优先把它加到搜索路径
if (Protobuf_ROOT)
    list(PREPEND CMAKE_PREFIX_PATH "${Protobuf_ROOT}")
endif()

# 优先使用 CONFIG 模式
find_package(Protobuf CONFIG QUIET)

if (Protobuf_FOUND)
    # 导出 dreamdb::protobuf
    if (TARGET protobuf::libprotobuf)
        add_library(dreamdb::protobuf ALIAS protobuf::libprotobuf)
    elseif (TARGET protobuf::protobuf)
        add_library(dreamdb::protobuf ALIAS protobuf::protobuf)
    else()
        message(FATAL_ERROR "Protobuf found (CONFIG), but no protobuf library target exported.")
    endif()

    # protoc target
    if (TARGET protobuf::protoc)
        add_executable(dreamdb::protoc ALIAS protobuf::protoc)
    else()
        # 某些包不导出 protoc target，则退化为找程序
        find_program(_DREAMDB_PROTOC protoc HINTS "${Protobuf_ROOT}/bin" REQUIRED)
        add_executable(dreamdb::protoc IMPORTED)
        set_target_properties(dreamdb::protoc PROPERTIES IMPORTED_LOCATION "${_DREAMDB_PROTOC}")
    endif()

    return()
endif()

# 再退化到 module 模式（FindProtobuf）
find_package(Protobuf REQUIRED)

# module 模式一般给变量：Protobuf_LIBRARIES / Protobuf_INCLUDE_DIRS / Protobuf_PROTOC_EXECUTABLE
add_library(dreamdb::protobuf INTERFACE IMPORTED)
target_include_directories(dreamdb::protobuf INTERFACE ${Protobuf_INCLUDE_DIRS})
target_link_libraries(dreamdb::protobuf INTERFACE ${Protobuf_LIBRARIES})

if (Protobuf_PROTOC_EXECUTABLE)
    add_executable(dreamdb::protoc IMPORTED)
    set_target_properties(dreamdb::protoc PROPERTIES
        IMPORTED_LOCATION "${Protobuf_PROTOC_EXECUTABLE}"
    )
else()
    find_program(_DREAMDB_PROTOC protoc HINTS "${Protobuf_ROOT}/bin" REQUIRED)
    add_executable(dreamdb::protoc IMPORTED)
    set_target_properties(dreamdb::protoc PROPERTIES IMPORTED_LOCATION "${_DREAMDB_PROTOC}")
endif()
