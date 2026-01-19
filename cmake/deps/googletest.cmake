# ============================================================================
#  Google Test
# ============================================================================

if (TARGET gtest OR TARGET gmock OR TARGET GTest::gtest OR TARGET GTest::gmock)
  return()
endif()

message(STATUS "Adding googletest (third_party/googletest)")

add_subdirectory(
    ${PROJECT_SOURCE_DIR}/third_party/googletest
    ${PROJECT_BINARY_DIR}/third_party/googletest
    EXCLUDE_FROM_ALL
)
