#pragma once

#include <limits> // for _GLIBCXX_NOEXCEPT, _NOEXCEPT

#if defined(__MACH__)
#   define _PMM_NOEXCEPT _NOEXCEPT
#elif defined(__linux__)
#   define _PMM_NOEXCEPT _GLIBCXX_NOEXCEPT
#else
#   error "Can only compile on MacOS and Linux"
#endif

#include <iostream>

#define PRINT(X) std::cout << X
#define __FLUSH(X) ;std::cout << std::endl

#define ERRLOG(X) std::cerr << X
#define __ERRFLUSH(X) ;std::cerr << std::endl

#ifndef IntVector
#   define IntVector std::vector<int>
#endif // !IntVector

#ifndef IntList
#   define IntList std::list<int>
#endif // !IntList

namespace PmergeMe {
    bool initInternals(const char *numList[]) _PMM_NOEXCEPT;
    void printInternalV(void) _PMM_NOEXCEPT;
    void printInternalL(void) _PMM_NOEXCEPT;

    bool testValidInputs(void);
    bool testEmptyInput(void);
    bool testEmptyString(void);
    bool testInvalidFormat(void);
    bool testNegativeNumber(void);
    bool testDuplicateValue(void);
    bool testWhitespaceHandling(void);
    bool testLargeNumbers(void);
    bool testOrderPreservation(void);
    bool testMixedValidInvalid(void);

    bool runAllTests(void);
}