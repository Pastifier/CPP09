#include "PmergeMe.hpp"

int main(int argc, char *argv[]) {
#if defined(_PMM_UNIT_TEST)
	(void)argc; (void)argv;
	PmergeMe::runAllTests();
	PRINT("Congratulations! All test cases have passed!") __FLUSH();
#else
	if (argc < 2) {
		ERRLOG("Error: Not enough arguments") __ERRFLUSH();
		ERRLOG("Usage:\n\tRPN x1 x2 ... xn") __ERRFLUSH();
		return 2;
	}

	const char **numList = const_cast<const char**>(argv + 1);

	if (!PmergeMe::initInternals(numList))
		return 1;
	
	PRINT("Before: "); PmergeMe::printInternalV();

	PmergeMe::mergeInsertionSortV();
	PmergeMe::mergeInsertionSortL();

#	if defined(_PMM_ASSERT_TEST)
		PRINT("After (vector): "); PmergeMe::printInternalV();
		PRINT("After (list): "); PmergeMe::printInternalL();
#	else
		PRINT("After: "); PmergeMe::printInternalV();
		PmergeMe::printTimeV();
		PmergeMe::printTimeL();
#	endif

#endif
	return 0;
}
