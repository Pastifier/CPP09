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

    bool isValidDate_impl(std::string const& date);

    bool isValidValue_impl(std::string const& value, double& result, int& errorCode);

    std::string trimWhitespace_impl(std::string const& str);

public:
    bool loadDatabase(std::string const& filename);
    void processInput(std::string const& filename);
};
