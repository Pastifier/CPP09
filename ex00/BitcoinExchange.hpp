#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <cstdlib>
#include <iomanip>

class BitcoinExchange {
private:
    std::map<std::string, double> database;

    bool isValidDate(const std::string& date);

    bool isValidValue(const std::string& value, double& result);

public:
    bool loadDatabase(const std::string& filename);
    void processInput(const std::string& filename);
};
