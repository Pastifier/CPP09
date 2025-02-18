#include "PmergeMe.hpp"
#include <string>
#include <sstream>
#include <cstdlib>
#include <exception>
#include <ctime>
#include <climits>
#include <algorithm>
#include <cassert>

namespace PmergeMe {
    namespace {
        IntVector _nsInternalV;
        IntList _nsInternalL;
        size_t _nsInternalCompCount = 0;

        clock_t _nsInternalElapsedTicksV = 0;
        clock_t _nsInternalElapsedTicksL = 0;

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

    size_t __binarySearchInsertionPositionInRange(const IntList& lst, int value, size_t low, size_t high) {
        size_t l = low;
        size_t h = high;
        
        while (l < h) {
            size_t mid = (l + h) / 2;
            
            IntList::const_iterator it = lst.begin();
            std::advance(it, mid);
            
            if (*it < value) {
                l = mid + 1;
            } else {
                h = mid;
            }
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
        size_t current = 2;
        for (size_t i = 1; i < jacobsthal.size(); ++i) {
            size_t start = current;
            size_t end = jacobsthal[i]; // inclusive

            for (size_t j = end; j >= start; --j) {
                order.push_back(j);
                if (j == start)
                    break;
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

    // 0 1 2 3 4
    // x y z w a

    // 0 1 2 3 4    0 1 2 3 4
    // y z w x a    l o p q r

    IntVector __extractCorrespondingLosers(const IntVector &winners, const IntVector &sortedWinners, const IntVector &losers) {
        // winners: original winners in pairing order.
        // sortedWinners: winners sorted recursively.
        // losers: losers paired with winners (in the same order as winners).
        IntVector pendChain;
        pendChain.resize(sortedWinners.size());

        for (size_t i = 0; i < sortedWinners.size(); ++i) {
            size_t index = std::distance(winners.begin(),
            std::find(winners.begin(), winners.end(), sortedWinners[i]));
            pendChain[i] = losers[index];
        }

        if (losers.size() > winners.size()) {
            pendChain.push_back(losers.back());
        }

#if defined(_PMM_ASSERT_TEST)
        for (size_t i = 0; i < sortedWinners.size(); ++i) {
            __myAssert(sortedWinners[i] >= pendChain[i]);
        }
#endif

        return pendChain;
    }

    IntVector __fordJohnsonSortV(const IntVector& input) {
        int n = input.size();
        if (n == 1) return input;
    
        IntVector winners;
        IntVector losers;
        IntVector partnerValue;
        
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
        } if (n % 2 != 0) {
            losers.push_back(input[n - 1]);
        }
        
        IntVector sortedWinners = __fordJohnsonSortV(winners);
        IntVector mainChain = sortedWinners;
        
        IntVector pendChain = __extractCorrespondingLosers(winners, sortedWinners, losers);
    
        if (!pendChain.empty()) {
            int firstLoser = pendChain[0];

            mainChain.insert(mainChain.begin(), firstLoser);
        }
    
        size_t pendingCount = pendChain.size();
        if (pendingCount > 1) {
            std::vector<size_t> insOrder = __generateInsertionOrder(pendingCount);
    
            for (size_t k = 0; k < insOrder.size(); ++k) {
                size_t idx = insOrder[k] - 1;
                if (idx == 0) continue;
                
                int loserVal = pendChain[idx];
                int partner = sortedWinners[idx];
                
                IntVector::iterator it = std::find(mainChain.begin(), mainChain.end(), partner);
                size_t partnerPos = it - mainChain.begin();
                
                size_t pos = __binarySearchInsertionPositionInRange(mainChain, loserVal, 0, partnerPos);
                mainChain.insert(mainChain.begin() + pos, loserVal);
            }
        }
        
        return mainChain;
    }

    IntList __extractCorrespondingLosers(const IntList& winners, const IntList& sortedWinners, const IntList& losers) {
        IntList pendChain;
        
        for (IntList::const_iterator sortedIt = sortedWinners.begin(); sortedIt != sortedWinners.end(); ++sortedIt) {
            IntList::const_iterator winnerIt = std::find(winners.begin(), winners.end(), *sortedIt);
            size_t index = std::distance(winners.begin(), winnerIt);
            
            IntList::const_iterator loserIt = losers.begin();
            std::advance(loserIt, index);
            pendChain.push_back(*loserIt);
        }
    
        if (losers.size() > winners.size()) {
            pendChain.push_back(losers.back());
        }

#if defined(_PMM_ASSERT_TEST)
        IntList::const_iterator pendIt = pendChain.begin();
        for (IntList::const_iterator sortedIt2 = sortedWinners.begin(); sortedIt2 != sortedWinners.end(); ++sortedIt2) {
            __myAssert(*sortedIt2 >= *pendIt);
            std::advance(pendIt, 1);
        }
#endif 
    
        return pendChain;
    }
    
    IntList __fordJohnsonSortL(const IntList& input) {
        size_t n = input.size();
        if (n == 1)
            return input;
    
        IntList winners;
        IntList losers;
        IntList partnerValue;
        
        IntList::const_iterator it = input.begin();
        while (it != input.end() && std::distance(it, input.end()) >= 2) {
            int first = *it;
            ++it;
            int second = *it;
            ++it;
            
            if (first < second) {
                winners.push_back(second);
                losers.push_back(first);
                partnerValue.push_back(second);
            } else {
                winners.push_back(first);
                losers.push_back(second);
                partnerValue.push_back(first);
            }
            ++_nsInternalCompCount;
        }
        
        if (it != input.end()) {
            losers.push_back(*it);
            partnerValue.push_back(std::numeric_limits<int>::max());
        }
        
        IntList sortedWinners = __fordJohnsonSortL(winners);
        IntList mainChain = sortedWinners;
    
        IntList pendChain = __extractCorrespondingLosers(winners, sortedWinners, losers);
        IntList sortedPartners = __extractCorrespondingLosers(winners, sortedWinners, partnerValue);
    
        if (!pendChain.empty()) {
            IntList::const_iterator pendIt = pendChain.begin();
            int firstLoser = *pendIt;

            mainChain.insert(mainChain.begin(), firstLoser);
        }
    
        size_t pendingCount = pendChain.size();
        if (pendingCount > 1) {
            std::vector<size_t> insOrder = __generateInsertionOrder(pendingCount);
    
            for (size_t k = 0; k < insOrder.size(); ++k) {
                size_t idx = insOrder[k] - 1;
                if (idx == 0)
                    continue;
                    
                IntList::const_iterator pendIt = pendChain.begin();
                IntList::const_iterator partnerIt = sortedPartners.begin(); ////
                std::advance(pendIt, idx);
                std::advance(partnerIt, idx);
                
                int loserVal = *pendIt;
                int partner = *partnerIt;
    
                IntList::iterator mainIt = std::find(mainChain.begin(), mainChain.end(), partner);
                size_t partnerPos = std::distance(mainChain.begin(), mainIt);
                
                size_t pos = __binarySearchInsertionPositionInRange(mainChain, loserVal, 0, partnerPos);
                mainIt = mainChain.begin();
                std::advance(mainIt, pos);
                mainChain.insert(mainIt, loserVal);
            }
        }

        return mainChain;
    }


    void mergeInsertionSortV(void) {
        clock_t start = clock();
        
        IntVector temp = __fordJohnsonSortV(_nsInternalV);
        
        clock_t end = clock();
        _nsInternalElapsedTicksV = end - start;
        
        _nsInternalV = temp;

#if defined(_PMM_ASSERT_TEST)
        PRINT("Vector Comparisons: " << _nsInternalCompCount) __FLUSH();
#endif
        _nsInternalCompCount = 0;
    }

    void mergeInsertionSortL(void) {
        clock_t start = clock();
        
        IntList temp = __fordJohnsonSortL(_nsInternalL);
        
        clock_t end = clock();
        _nsInternalElapsedTicksL = end - start;
        
        _nsInternalL = temp;

#if defined(_PMM_ASSERT_TEST)
        PRINT("List Comparisons: " << _nsInternalCompCount) __FLUSH();
#endif
        _nsInternalCompCount = 0;
    } 

    void printTimeV(void) {
		PRINT("Time to process a range of " << _nsInternalV.size() << " elements with std::vector : " << _nsInternalElapsedTicksV << " ticks.") __FLUSH();
    }

    void printTimeL(void) {
		PRINT("Time to process a range of " << _nsInternalL.size() << " elements with std::list : " << _nsInternalElapsedTicksL << " ticks.") __FLUSH();
    }
}