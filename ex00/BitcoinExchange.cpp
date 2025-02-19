#include "BitcoinExchange.hpp"

bool BitcoinExchange::isValidDate(const std::string& date) {
    if (date.empty()) return false;
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7) {
            if (!isdigit(date[i])) return false;
        }
    }

    std::istringstream iss(date);
    int year, month, day;
    char dash1, dash2;
    
    iss >> year >> dash1 >> month >> dash2 >> day;
    
    if (iss.fail() || !iss.eof()) return false;
    
    if (year < 0) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (day > (isLeap ? 29 : 28)) return false;
    }
    
    return true;
}

enum e_error_codes {
    NO_ERROR,
    EMPTY_STRING,
    INVALID_FORMAT, // wrong position of - or ., multiple decimals, etc
    NON_NUMERIC,
    NEGATIVE_NUMBER,
    TOO_LARGE,
    LEADING_ZEROES
};

bool BitcoinExchange::isValidValue(const std::string& value, double& result, int& errorCode) {
    errorCode = 0;
    if (value.empty()) {
        errorCode = 1;
        return false;
    }

    for (std::string::size_type i = 0; i < value.length(); ++i) {
        if (!isdigit(value[i]) && value[i] != '.' && value[i] != '-') {
            errorCode = 3;
            return false;
        }
    }

    bool hasDecimal = false;
    bool hasDigit = false;
    int decimalPosition = -1;
    
    for (std::string::size_type i = 0; i < value.length(); ++i) {
        char c = value[i];
        
        if (c == '.') {
            if (hasDecimal || i == 0 || i == value.length() - 1) {
                errorCode = 2;
                return false;
            }
            hasDecimal = true;
            decimalPosition = i;
        }
        
        if (c == '-') {
            if (i != 0) {
                errorCode = 2;
                return false;
            }
        }
        
        if (isdigit(c)) hasDigit = true;
    }
    
    if (!hasDigit) {
        errorCode = 2;
        return false;
    }
    
    if (value.length() > 1 && value[0] == '0' && 
        (decimalPosition != 1 || decimalPosition == -1)) {
        errorCode = 6;
        return false;
    }

    char* endPtr;
    const char* str = value.c_str();
    
    result = strtod(str, &endPtr);
    
    if (*endPtr != '\0' || endPtr == str) {
        errorCode = 2;
        return false;
    }
    
    if (result < 0) {
        errorCode = 4;
        return false;
    }
    
    if (result > 1000) {
        errorCode = 5;
        return false;
    }
    
    return true;
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
            std::istringstream middleMan(value);
            middleMan >> rate;
            database[date] = rate;
        }
    }
    return !database.empty();
}

void BitcoinExchange::processInput(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    if (!std::getline(file, line) || line != "date | value") {
        std::cerr << "Error: invalid header format." << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty()) {
            std::cerr << "Error: empty line." << std::endl;
            continue;
        }

        size_t pipeCount = 0;
        for (std::string::size_type i = 0; i < line.length(); ++i) {
            if (line[i] == '|') pipeCount++;
        }
        if (pipeCount != 1) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::istringstream iss(line);
        std::string date, value;

        if (!std::getline(iss, date, '|')) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        if (!std::getline(iss, value)) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::string::size_type notwhite = date.find_first_not_of(" \t");
        if (notwhite == std::string::npos) {
            std::cerr << "Error: empty date field." << std::endl;
            continue;
        }
        date.erase(0, notwhite);
        notwhite = date.find_last_not_of(" \t");
        if (notwhite != std::string::npos) {
            date.erase(notwhite + 1);
        }

        notwhite = value.find_first_not_of(" \t");
        if (notwhite == std::string::npos) {
            std::cerr << "Error: empty value field." << std::endl;
            continue;
        }
        value.erase(0, notwhite);
        notwhite = value.find_last_not_of(" \t");
        if (notwhite != std::string::npos) {
            value.erase(notwhite + 1);
        }

        if (!isValidDate(date)) {
            std::cerr << "Error: bad input => " << date << std::endl;
            continue;
        }

        double amount;
        int errorCode;
        if (!isValidValue(value, amount, errorCode)) {
            switch (errorCode) {
                case EMPTY_STRING:
                    std::cout << "Error: empty value." << std::endl;
                    break;
                case INVALID_FORMAT:
                    std::cout << "Error: bad input => " << value << std::endl;
                    break;
                case NON_NUMERIC:
                    std::cout << "Error: invalid value (non-numerical input)." << std::endl;
                    break;
                case NEGATIVE_NUMBER:
                    std::cout << "Error: not a positive number." << std::endl;
                    break;
                case TOO_LARGE:
                    std::cout << "Error: too large a number." << std::endl;
                    break;
                case LEADING_ZEROES:
                    std::cout << "Error: bad input => " << value << std::endl;
                    break;
            }
            continue;
        }

        std::map<std::string, double>::iterator it = database.upper_bound(date);
        if (it == database.begin()) {
            std::cerr << "Error: no exchange rate available for this date." << std::endl;
            continue;
        }
        --it;
        std::cout << "DATE: " << it->first << std::endl;
        double result = amount * it->second;
        std::cout << date << " => " << amount << " = " << result << std::endl;
    }
}
