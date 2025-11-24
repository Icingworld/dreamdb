#include <cassert>

// Simple test helper macro
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Test failed: " << message << " (file: " << __FILE__ << ", line: " << __LINE__ << ")" << std::endl; \
            throw std::runtime_error(message); \
        } \
    } while(0)

// Test if exception is thrown
#define TEST_EXCEPTION(statement, exception_type, message) \
    do { \
        bool exception_thrown = false; \
        try { \
            statement; \
        } catch (const exception_type &) { \
            exception_thrown = true; \
        } catch (...) { \
            std::cerr << "Test failed: " << message << " - unexpected exception type thrown" << std::endl; \
            throw std::runtime_error(message); \
        } \
        if (!exception_thrown) { \
            std::cerr << "Test failed: " << message << " - expected exception but none was thrown" << std::endl; \
            throw std::runtime_error(message); \
        } \
    } while(0)
