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
#include <vector>
#include <list>
#include <algorithm>

#define PRINT(X) std::cout << X
#define __FLUSH(X) ;std::cout << std::endl

#define ERRLOG(X) std::cerr << X
#define __ERRFLUSH(X) ;std::cerr << std::endl

typedef std::list<int> IntList;
typedef std::vector<int> IntVector;

typedef std::pair<IntVector, IntVector> IntVectorPair;

#define __PMM_SWAP_INT_PAIR_VALUES(X) X.first^=X.second;X.first^=X.second;X.first^=X.second

#define _PMM_PARSING_ONLY

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

    bool runAllTests(void) _PMM_PARSING_ONLY;

    void mergeInsertionSortV(void);
    void mergeInsertionSortL(void);
}