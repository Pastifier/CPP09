#include "RPN.hpp"
#include <cassert>
#include <iostream>

void assertExpression(const std::string& expr, int expected) {
    RPN::processExpression(expr);
    int result = RPN::getResult();
    assert(result == expected);
}

void test_single_operations() {
    std::cout << "Testing single operations...\n";
    
    assertExpression("3 4 +", 7);
    assertExpression("10 3 -", 7);
    assertExpression("4 5 *", 20);
    assertExpression("15 3 /", 5);
    
    std::cout << "Single operations passed!\n";
}

void test_complex_expressions() {
    std::cout << "Testing complex expressions...\n";
    
    assertExpression("3 4 + 5 *", 35);  // (3 + 4) * 5
    assertExpression("10 2 8 + -", 0);  // 10 - (2 + 8)
    assertExpression("4 5 * 2 3 * +", 26);  // (4 * 5) + (2 * 3)
    assertExpression("15 3 / 2 *", 10);  // (15 / 3) * 2
    
    std::cout << "Complex expressions passed!\n";
}

void test_error_handling() {
    std::cout << "Testing error conditions...\n";
    
    // Division by zero
    try {
        RPN::processExpression("5 0 /");
        assert(false && "Should have thrown DivisionByZeroError");
    } catch (const RPN::DivisionByZeroError&) {
        std::cout << "Division by zero caught correctly\n";
    }

    // Stack underflow - not enough operands
    try {
        RPN::processExpression("5 +");
        assert(false && "Should have thrown StackUnderflowError");
    } catch (const RPN::StackUnderflowError&) {
        std::cout << "Stack underflow caught correctly\n";
    }

    // Invalid token
    try {
        RPN::processExpression("5 x 4 +");
        assert(false && "Should have thrown InvalidTokenError");
    } catch (const RPN::InvalidTokenError& e) {
        std::cout << "Invalid token caught correctly\n";
    }

    // Extra operands
    try {
        RPN::processExpression("5 4 3 +");
        RPN::getResult();
        assert(false && "Should have thrown ExtraOperandsError");
    } catch (const RPN::ExtraOperandsError&) {
        std::cout << "Extra operands caught correctly\n";
    }

    std::cout << "Error handling passed!\n";
}

void test_edge_cases() {
    std::cout << "Testing edge cases...\n";
    
    // Empty expression
    try {
        RPN::processExpression("");
        RPN::getResult();
        assert(false && "Should have thrown StackUnderflowError");
    } catch (const RPN::StackUnderflowError&) {
        std::cout << "Empty expression caught correctly\n";
    }

    // Single number
    assertExpression("42", 42);

    // Multiple spaces between tokens
    assertExpression("3    4     +", 7);

    // Leading/trailing spaces
    assertExpression("   3 4 +   ", 7);

    std::cout << "Edge cases passed!\n";
}

#define PRINT(X) std::cout << X
#define ERRLOG(X) std::cerr << X
#define FLUSH() std::cout << std::endl

int main(int argc, char *argv[]) {
    try {
#ifdef _RPN_UNIT_TEST
        (void)argc; (void)argv;
        test_single_operations();
        test_complex_expressions();
        test_error_handling();
        test_edge_cases();
        
        PRINT("\nAll tests passed successfully!"); FLUSH();
#else
        if (argc != 2) {
            ERRLOG("Usage:\n");
            ERRLOG("\tRPN <expression-string>"); FLUSH();
            return 2;
        }

        std::string expr(argv[1]);
        RPN::processExpression(expr);
        PRINT(RPN::getResult()); FLUSH();
#endif
        return 0;
    } catch (const std::exception& e) {
        ERRLOG("Unexpected error: " << e.what()); FLUSH();
        return 1;
    } catch (...) {
        ERRLOG("Unknown error occurred"); FLUSH();
        return 1;
    }
}
