#include "PmergeMe.hpp"

int main(int argc, char *argv[]) {
#if defined(_PMM_UNIT_TEST)
	(void)argc; (void)argv;
	PmergeMe::runAllTests();
	PRINT("Congratulations! All test cases have passed!") __FLUSH();
#else
	if (argc < 3) {
		ERRLOG("Error: Not enough arguments") __ERRFLUSH();
		ERRLOG("Usage:\n\tRPN x1 x2 ... xn") __ERRFLUSH();
		return 2;
	}
	const char **numList = const_cast<const char**>(argv + 1);
	if (!PmergeMe::initInternals(numList))
		return 1;
	PmergeMe::printInternalV();
	PmergeMe::printInternalL();
#endif
	return 0;
}
