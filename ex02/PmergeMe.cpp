#include "PmergeMe.hpp"
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <exception>

namespace PmergeMe {
    std::vector<int> _nsInternalV;
    std::list<int> _nsInternalL;

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

            if (std::find(_nsInternalV.begin(), _nsInternalV.end(), toAppend) != _nsInternalV.end()) { // Cache goes brrrrrr
                errorCode = DUPLICATE_VALUE;
                return false;
            }

            _nsInternalL.push_back(toAppend);
            _nsInternalV.push_back(toAppend);
            
            return true;
        } catch (const std::ios_base::failure& f) {
            errorCode = STREAM_FAILURE;
            ERRLOG(f.what()) __ERRFLUSH();
            return false;
        }
    }

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
                    case DUPLICATE_VALUE:
                        ERRLOG("Error: `" << token << "` already exists and is not allowed.") __ERRFLUSH();
                        break;
                    default:
                        ERRLOG("Status: Irrecoverable error encountered.") __ERRFLUSH();
                }
                return false;
            }
        }
        return true;
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
        
        std::vector<int> listValues(_nsInternalL.begin(), _nsInternalL.end());
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

    bool runAllTests(void) {
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
}