#include "PmergeMe.hpp"
#include <string>
#include <sstream>
#include <cstdlib>
#include <exception>
#include <climits>
#include <algorithm>

namespace PmergeMe {
    namespace {
        IntVector _nsInternalV;
        IntList _nsInternalL;
        size_t _nsInternalCompCount = 0;

        enum e_error_codes {
            NO_ERROR,
            EMPTY_STRING,
            INVALID_FOMRAT,
            NEGATIVE_NUMBER,
            DUPLICATE_VALUE,
            STREAM_FAILURE
        };

        bool __parsePushValue(std::string const &token, int &errorCode) _PMM_NOEXCEPT {
            if (token.empty()) {
                errorCode = EMPTY_STRING;
                return false;
            }

            try {
                std::istringstream iss(token);
                int toAppend;
                
                if (!(iss >> toAppend) || !(iss >> std::ws).eof()) {
                    errorCode = INVALID_FOMRAT;
                    return false;
                }

                if (toAppend < 0) {
                    errorCode = NEGATIVE_NUMBER;
                    return false;
                }

                // if (std::find(_nsInternalV.begin(), _nsInternalV.end(), toAppend) != _nsInternalV.end()) { // Cache goes brrrrrr
                //     errorCode = DUPLICATE_VALUE;
                //     return false;
                // }

                _nsInternalL.push_back(toAppend);
                _nsInternalV.push_back(toAppend);

                return true;
            } catch (const std::ios_base::failure& f) {
                errorCode = STREAM_FAILURE;
                ERRLOG(f.what()) __ERRFLUSH();

                return false;
            }
        }

        void __setUp(void) {
            _nsInternalV.clear();
            _nsInternalL.clear();
        }

        void __tearDown(void) {
            _nsInternalV.clear();
            _nsInternalL.clear();
        }

        void __myAssert(bool expr_) {
            if (!expr_)
                throw std::runtime_error("Check line: ");
        }
    }

    bool testValidInputs(void) {
        __setUp();
        const char* input[] = {"1", "2", "3", NULL};
        bool result = initInternals(input);
        __myAssert(result == true);
        __myAssert(_nsInternalV.size() == 3);
        __myAssert(_nsInternalL.size() == 3);
        __tearDown();
        return true;
    }

    bool testEmptyString(void) {
        __setUp();
        const char* input[] = {"1", "", "3", NULL};
        bool result = initInternals(input);
        __myAssert(result == true);
        __myAssert(_nsInternalV.size() == 2);
        __myAssert(_nsInternalL.size() == 2);
        __tearDown();
        return true;
    }

    bool testInvalidFormat(void) {
        __setUp();
        const char* input[] = {"1", "2a", "3", NULL};
        bool result = initInternals(input);
        __myAssert(result == false);
        __myAssert(_nsInternalV.size() == 1);
        __myAssert(_nsInternalL.size() == 1);
        __tearDown();
        return true;
    }

    bool testNegativeNumber(void) {
        __setUp();
        const char* input[] = {"1", "-2", "3", NULL};
        bool result = initInternals(input);
        __myAssert(result == false);
        __myAssert(_nsInternalV.size() == 1);
        __myAssert(_nsInternalL.size() == 1);
        __tearDown();
        return true;
    }

    bool testDuplicateValue(void) {
        __setUp();
        const char* input[] = {"1", "2", "2", NULL};
        bool result = initInternals(input);
        __myAssert(result == false);
        __myAssert(_nsInternalV.size() == 2);
        __myAssert(_nsInternalL.size() == 2);
        __tearDown();
        return true;
    }

    bool testWhitespaceHandling(void) {
        __setUp();
        const char* input[] = {"1", " 2 ", "3", NULL};
        bool result = initInternals(input);
        __myAssert(result == true);
        __myAssert(_nsInternalV.size() == 3);
        __myAssert(_nsInternalL.size() == 3);
        __tearDown();
        return true;
    }

    bool testLargeNumbers(void) {
        __setUp();
        const char* input[] = {"2147483647", "1", "2", NULL};
        bool result = initInternals(input);
        __myAssert(result == true);
        __myAssert(_nsInternalV.size() == 3);
        __myAssert(_nsInternalL.size() == 3);
        __tearDown();
        return true;
    }

    bool testOrderPreservation(void) {
        __setUp();
        const char* input[] = {"5", "3", "1", NULL};
        bool result = initInternals(input);
        __myAssert(result == true);
        __myAssert(_nsInternalV[0] == 5);
        __myAssert(_nsInternalV[1] == 3);
        __myAssert(_nsInternalV[2] == 1);
        
        IntVector listValues(_nsInternalL.begin(), _nsInternalL.end());
        __myAssert(listValues[0] == 5);
        __myAssert(listValues[1] == 3);
        __myAssert(listValues[2] == 1);
        __tearDown();
        return true;
    }

    bool testMixedValidInvalid(void) {
        __setUp();
        const char* input[] = {"1", "abc", "3", "-4", "5", NULL};
        bool result = initInternals(input);
        __myAssert(result == false);
        __myAssert(_nsInternalV.size() == 1);
        __myAssert(_nsInternalL.size() == 1);
        __tearDown();
        return true;
    }

    bool runAllTests(void) _PMM_PARSING_ONLY {
        bool allPassed = true;
        
        try {
            allPassed &= testValidInputs();
            allPassed &= testEmptyString();
            allPassed &= testInvalidFormat();
            allPassed &= testNegativeNumber();
            allPassed &= testDuplicateValue();
            allPassed &= testWhitespaceHandling();
            allPassed &= testLargeNumbers();
            allPassed &= testOrderPreservation();
            allPassed &= testMixedValidInvalid();
        } catch (const std::exception& e) {
            ERRLOG("Test failed with exception: " << e.what()) __ERRFLUSH();
            return false;
        }
        
        return allPassed;
    }

    // template <typename T>
    // void __mergeInsertionGeneric(T& c, int depth);

    void printInternalV(void) _PMM_NOEXCEPT {
        IntVector::const_iterator it = _nsInternalV.begin();

        for (IntVector::const_iterator i = it; i != _nsInternalV.end(); ++i) {
            PRINT(*i << (i + 1 == _nsInternalV.end() ? "" : " "));
        } __FLUSH();
    }

    void printInternalL(void) _PMM_NOEXCEPT {
        IntList::const_iterator it = _nsInternalL.begin();

        for (IntList::const_iterator i = it; i != _nsInternalL.end(); ++i) {
            PRINT(*i << (i == _nsInternalL.end() ? "" : " "));
        } __FLUSH();
    }

    bool initInternals(const char *numList[]) _PMM_NOEXCEPT {
        int errorCode = 0;

        for (int i = 0; numList[i]; ++i) {
            std::string token = numList[i];
            if (!__parsePushValue(token, errorCode)) {
                switch (errorCode) {
                    case EMPTY_STRING:
                        ERRLOG("Warning: Empty string encountered. Moving on...") __ERRFLUSH();
                        continue;
                    case INVALID_FOMRAT:
                        ERRLOG("Error: `" << token << "`: Invalid format.") __ERRFLUSH();
                        break;
                    case NEGATIVE_NUMBER:
                        ERRLOG("Error: `" << token << "`: Only accepting positive integers.") __ERRFLUSH();
                        break;
                    // case DUPLICATE_VALUE:
                    //     ERRLOG("Error: `" << token << "` already exists and is not allowed.") __ERRFLUSH();
                    //     break;
                    default:
                        ERRLOG("Status: Irrecoverable error encountered.") __ERRFLUSH();
                }
                return false;
            }
        }

        if (_nsInternalV.size() < 2) {
            ERRLOG("Error: Not enough input elements. Need at least two positive integers.") __ERRFLUSH();
            return false;
        }
        return true;
    }

    // Binary search in range [low, high) of vec.
    size_t __binarySearchInsertionPositionInRange(const IntVector& vec, int value, size_t low, size_t high) {
        size_t l = low, h = high;
        while (l < h) {
            size_t mid = (l + h) / 2;
            if (vec[mid] < value)
                l = mid + 1;
            else
                h = mid;
            ++_nsInternalCompCount;
        }
        return l;
    }

    std::vector<size_t> __generateInsertionOrder(size_t n) {
        std::vector<size_t> jacobsthal;
        jacobsthal.push_back(1); // t1 = 1
        while (true) {
            size_t k = jacobsthal.size();
            size_t next;
            if (k == 1)
                next = 3; // t2 = 3
            else
                next = jacobsthal[k - 1] + 2 * jacobsthal[k - 2];
            if (next > n)
                break;
            jacobsthal.push_back(next);
        }
        
        std::vector<size_t> order;
        size_t current = 2; // pending position 1 is inserted separately
        for (size_t i = 1; i < jacobsthal.size(); ++i) {
            size_t start = current;
            size_t end = jacobsthal[i]; // inclusive

            for (size_t j = end; j >= start; --j) {
                order.push_back(j);
                // if (j == start)
                //     break;
            }
            current = jacobsthal[i] + 1;
        }
        if (current <= n) {
            for (size_t j = n; j >= current; --j) {
                order.push_back(j);
                if (j == current)
                    break;
            }
        }
        return order;
    }

    IntVector __fordJohnsonSortV(const IntVector& input) {
        int n = input.size();
        if (n <= 1)
            return input;

        IntVector winners;
        IntVector losers;
        IntVector partnerValue;  // For each pending element, store its paired winner.
        
        for (int i = 0; i + 1 < n; i += 2) {
            if (input[i] < input[i + 1]) {
                winners.push_back(input[i + 1]);
                losers.push_back(input[i]);
                partnerValue.push_back(input[i + 1]);
            } else {
                winners.push_back(input[i]);
                losers.push_back(input[i + 1]);
                partnerValue.push_back(input[i]);
            }
            ++_nsInternalCompCount;
        } if (n % 2 == 1) {
            losers.push_back(input[n - 1]);
            partnerValue.push_back(std::numeric_limits<int>::max());
        }
        
        IntVector sortedWinners = __fordJohnsonSortV(winners);
        IntVector mainChain = sortedWinners;

        if (!losers.empty()) {
            int firstLoser = losers[0];
            int partner = partnerValue[0];

            int partnerPos = 0;
            {
                IntVector::const_iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), partner);
                partnerPos = it - mainChain.begin();
            }
            int pos = __binarySearchInsertionPositionInRange(mainChain, firstLoser, 0, partnerPos);
            mainChain.insert(mainChain.begin() + pos, firstLoser);
        }

        
        int pendingCount = losers.size();
        if (pendingCount > 1) {
            std::vector<size_t> insOrder = __generateInsertionOrder(pendingCount);

            for (int k = 0; k < static_cast<int>(insOrder.size()); ++k) {
                int idx = insOrder[k] - 1;
                if (idx == 0)
                    continue;
                int loserVal = losers[idx];
                int partner = partnerValue[idx];

                int partnerPos = 0;
                {
                    IntVector::const_iterator it = std::lower_bound(mainChain.begin(), mainChain.end(), partner);
                    partnerPos = it - mainChain.begin();
                }
                int pos = __binarySearchInsertionPositionInRange(mainChain, loserVal, 0, partnerPos);
                mainChain.insert(mainChain.begin() + pos, loserVal);
            }
        }
        
        return mainChain;
    }



    void mergeInsertionSortV(void) {
        IntVector temp = __fordJohnsonSortV(_nsInternalV);
        _nsInternalV = temp;
        PRINT("Vector Comparisons: " << _nsInternalCompCount) __FLUSH();
        _nsInternalCompCount = 0;
    }
}