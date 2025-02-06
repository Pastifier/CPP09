#include "BitcoinExchange.hpp"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Error: could not open file." << std::endl;
        return 1;
    }

    BitcoinExchange exchange;
    if (!exchange.loadDatabase("data.csv")) {
        std::cout << "Error: could not open database file." << std::endl;
        return 1;
    }

    exchange.processInput(argv[1]);
    return 0;
}