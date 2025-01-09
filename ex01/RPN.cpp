#include "RPN.hpp"
#include <stack>
#include <cstring>
#include <cstdlib>

namespace RPN {
    namespace {
        std::stack<int> _nsInternalS;

        void clearStack() {
            while (!_nsInternalS.empty()) {
                _nsInternalS.pop();
            }
        }

        void ensureOperands() {
            if (_nsInternalS.size() < 2) {
                throw StackUnderflowError();
            }
        }

        std::pair<int, int> popOperands() {
            ensureOperands();
            int b = _nsInternalS.top();
            _nsInternalS.pop();
            int a = _nsInternalS.top();
            _nsInternalS.pop();
            std::pair<int, int> result(a, b);
            return result;
        }

        int __iadd(int a, int b) { return a + b; }
        int __isub(int a, int b) { return a - b; }
        int __imul(int a, int b) { return a * b; }
        int __idiv(int a, int b) {
            if (b == 0) throw DivisionByZeroError();
            return a / b;
        }

        bool isOperator(char c) {
            return (c == '+' || c == '-' ||
                    c == '*' || c == '/');
        }

        bool isValidNumber(const std::string& token) {
            if (token.empty()) return false;
            
            const char* str = token.c_str();
            char* endptr;
            std::strtol(str, &endptr, 10);
            return (*endptr == '\0');
        }
    }

    void processExpression(std::string const & expr) {
        clearStack();
        const char *left = expr.c_str();
        const char *right;

        while (left && *left) {
            while (*left == ' ') left++;
            if (!*left) break;

            right = std::strpbrk(left, " ");

            std::string token;
            if (right) {
                token = std::string(left, right - left);
                left = right + 1;
            } else {
                token = std::string(left);
                left = NULL;
            }

            if (token.empty()) continue;

            if (isValidNumber(token)) {
                _nsInternalS.push(std::atoi(token.c_str()));
            } else if (token.length() == 1 && isOperator(token[0])) {
                std::pair<int, int> operands = popOperands();
                int a = operands.first;
                int b = operands.second;
                
                switch (token[0]) {
                case '+': _nsInternalS.push(__iadd(a, b)); break;
                case '-': _nsInternalS.push(__isub(a, b)); break;
                case '*': _nsInternalS.push(__imul(a, b)); break;
                case '/': _nsInternalS.push(__idiv(a, b)); break;
                }
            } else {
                throw InvalidTokenError(token);
            }
        }
    }

    int getResult() {
        if (_nsInternalS.empty()) {
            throw StackUnderflowError();
        }
        int result = _nsInternalS.top();
        _nsInternalS.pop();
        
        if (!_nsInternalS.empty()) {
            throw ExtraOperandsError();
        }
        
        return result;
    }
}
