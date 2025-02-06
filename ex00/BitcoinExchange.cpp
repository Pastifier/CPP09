#include "BitcoinExchange.hpp"

bool BitcoinExchange::isValidDate(const std::string& date) {
    if (date.empty()) return false;
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    std::istringstream iss(date);
    int year, month, day;
    char dash1, dash2;
    
    iss >> year >> dash1 >> month >> dash2 >> day;
    
    if (iss.fail() || !iss.eof()) return false;
    
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (day > (isLeap ? 29 : 28)) return false;
    }
    
    return true;
}

bool BitcoinExchange::isValidValue(const std::string& value, double& result) {
    if (value.empty()) return false;
    
    char* endPtr;
    const char* str = value.c_str();
    
    result = strtod(str, &endPtr);
    
    if (*endPtr != '\0' || endPtr == str) {
        return false;
    }
    
    return result >= 0 && result <= 1000;
}

bool BitcoinExchange::loadDatabase(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) return false;

    std::string line;
    if (!std::getline(file, line) || line != "date,exchange_rate") {
        return false;
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string date;
        std::string value;
        
        if (!std::getline(iss, date, ',')) continue;
        if (!std::getline(iss, value)) continue;
        
        if (isValidDate(date)) {
            double rate;
            if (isValidValue(value, rate)) {
                database[date] = rate;
            }
        }
    }
    return !database.empty();
}

void BitcoinExchange::processInput(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cout << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    if (!std::getline(file, line) || line != "date | value") {
        std::cout << "Error: invalid header format." << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty()) {
            std::cout << "Error: empty line." << std::endl;
            continue;
        }

        std::istringstream iss(line);
        std::string date, value;
        
        if (!std::getline(iss, date, '|')) {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        
        if (!std::getline(iss, value)) {
            std::cout << "Error: bad input => " << line << std::endl;
            continue;
        }
        
        std::string::size_type notwhite = date.find_first_not_of(" \t");
        if (notwhite == std::string::npos) {
            std::cout << "Error: empty date field." << std::endl;
            continue;
        }
        date.erase(0, notwhite);
        notwhite = date.find_last_not_of(" \t");
        if (notwhite != std::string::npos) {
            date.erase(notwhite + 1);
        }
        
        notwhite = value.find_first_not_of(" \t");
        if (notwhite == std::string::npos) {
            std::cout << "Error: empty value field." << std::endl;
            continue;
        }
        value.erase(0, notwhite);
        notwhite = value.find_last_not_of(" \t");
        if (notwhite != std::string::npos) {
            value.erase(notwhite + 1);
        }

        if (!isValidDate(date)) {
            std::cout << "Error: bad input => " << date << std::endl;
            continue;
        }

        double amount;
        if (!isValidValue(value, amount)) {
            if (value.find('-') != std::string::npos) {
                std::cout << "Error: not a positive number." << std::endl;
            } else {
                std::cout << "Error: too large a number." << std::endl;
            }
            continue;
        }

        std::map<std::string, double>::iterator it = database.upper_bound(date);
        if (it == database.begin()) {
            std::cout << "Error: no exchange rate available for this date." << std::endl;
            continue;
        }
        --it;
        double result = amount * it->second;
        std::cout << date << " => " << amount << " = " << result << std::endl;
    }
}
