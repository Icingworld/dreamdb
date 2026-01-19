# ============================================================================
# Asio (Head Only)
# ============================================================================

if(TARGET dreamdb::asio)
    return()
endif()

message(STATUS "Adding asio (third_party/asio)")

add_library(asio INTERFACE)

target_include_directories(asio INTERFACE
    ${PROJECT_SOURCE_DIR}/third_party/asio/include
)

add_library(dreamdb::asio ALIAS asio)
