#include "dreamdb/common/decimal.h"

#include <iostream>
#include <cassert>
#include <stdexcept>

using namespace dreamdb;

void test_basic_construction()
{
    std::cout << "Testing basic construction...\n";
    
    // 从 int64_t 构造
    Decimal d1(static_cast<std::int64_t>(12345), 5, 2);  // DECIMAL(5,2) = 123.45
    assert(d1.value() == 12345);
    assert(d1.precision() == 5);
    assert(d1.scale() == 2);
    
    // 从 double 构造
    Decimal d2(123.45, 5, 2);
    assert(d2.value() == 12345);
    
    // 从字符串构造
    Decimal d3("123.45", 5, 2);
    assert(d3.value() == 12345);
    
    std::cout << "  ✓ Basic construction passed\n";
}

void test_to_string()
{
    std::cout << "Testing to_string()...\n";
    
    Decimal d1(static_cast<std::int64_t>(12345), 5, 2);
    assert(d1.to_string() == "123.45");
    
    Decimal d2(static_cast<std::int64_t>(100), 3, 2);
    assert(d2.to_string() == "1.00");
    
    Decimal d3(static_cast<std::int64_t>(-12345), 5, 2);
    assert(d3.to_string() == "-123.45");
    
    Decimal d4(static_cast<std::int64_t>(5), 2, 1);
    assert(d4.to_string() == "0.5");
    
    std::cout << "  ✓ to_string() passed\n";
}

void test_arithmetic()
{
    std::cout << "Testing arithmetic operations...\n";
    
    Decimal d1(static_cast<std::int64_t>(100), 3, 2);  // 1.00
    Decimal d2(static_cast<std::int64_t>(200), 3, 2);  // 2.00
    
    // 加法
    Decimal sum = d1 + d2;
    assert(sum.value() == 300);
    assert(sum.to_string() == "3.00");
    
    // 减法
    Decimal diff = d2 - d1;
    assert(diff.value() == 100);
    assert(diff.to_string() == "1.00");
    
    // 乘法
    Decimal prod = d1 * d2;
    assert(prod.value() == 20000);
    assert(prod.to_string() == "2.00");
    
    // 除法
    Decimal quot = d2 / d1;
    assert(quot.value() == 200);
    assert(quot.to_string() == "2.00");
    
    std::cout << "  ✓ Arithmetic operations passed\n";
}

void test_comparison()
{
    std::cout << "Testing comparison operations...\n";
    
    Decimal d1(static_cast<std::int64_t>(100), 3, 2);  // 1.00
    Decimal d2(static_cast<std::int64_t>(200), 3, 2);  // 2.00
    Decimal d3(static_cast<std::int64_t>(100), 3, 2);  // 1.00
    
    assert(d1 < d2);
    assert(d2 > d1);
    assert(d1 <= d2);
    assert(d2 >= d1);
    assert(d1 == d3);
    assert(d1 != d2);
    
    // 不同 scale 的比较
    Decimal d4(static_cast<std::int64_t>(1000), 4, 3);  // 1.000
    assert(d1 == d4);  // 1.00 == 1.000
    
    std::cout << "  ✓ Comparison operations passed\n";
}

void test_different_scales()
{
    std::cout << "Testing operations with different scales...\n";
    
    Decimal d1(static_cast<std::int64_t>(100), 3, 2);   // 1.00
    Decimal d2(static_cast<std::int64_t>(1000), 4, 3);  // 1.000
    
    Decimal sum = d1 + d2;
    assert(sum.to_string() == "2.000");
    
    std::cout << "  ✓ Different scales operations passed\n";
}

void test_errors()
{
    std::cout << "Testing error handling...\n";
    
    // 无效的 precision/scale
    try {
        Decimal d(static_cast<std::int64_t>(100), 0, 0);
        assert(false && "Should throw");
    } catch (const std::invalid_argument &) {
        // 预期
    }
    
    try {
        Decimal d(static_cast<std::int64_t>(100), 5, 6);
        assert(false && "Should throw");
    } catch (const std::invalid_argument &) {
        // 预期
    }
    
    // 除零
    Decimal d1(static_cast<std::int64_t>(100), 3, 2);
    Decimal d2(static_cast<std::int64_t>(0), 3, 2);
    try {
        Decimal result = d1 / d2;
        assert(false && "Should throw");
    } catch (const std::domain_error &) {
        // 预期
    }
    
    std::cout << "  ✓ Error handling passed\n";
}

int main()
{
    std::cout << "Running Decimal class tests...\n\n";
    
    try {
        test_basic_construction();
        test_to_string();
        test_arithmetic();
        test_comparison();
        test_different_scales();
        test_errors();
        
        std::cout << "\n✓ All tests passed!\n";
        return 0;
    } catch (const std::exception & e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}

