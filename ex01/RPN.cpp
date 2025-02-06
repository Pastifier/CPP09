#include "RPN.hpp"
#include <stack>
#include <cstring>
#include <cstdlib>

namespace RPN {

    RPNException::RPNException(const char* message) : _msg(message) {}
    RPNException::~RPNException() throw() {}
    const char* RPNException::what() const throw()  { return _msg; }

    DivisionByZeroError::DivisionByZeroError() : RPNException("Division by zero") {}
    DivisionByZeroError::~DivisionByZeroError() throw() {}
    StackUnderflowError::StackUnderflowError() : RPNException("Not enough operands") {}
    StackUnderflowError::~StackUnderflowError() throw() {}
    InvalidTokenError::InvalidTokenError(const std::string& token) 
        : RPNException("Invalid token encountered"), _token(token) {}
    const std::string& InvalidTokenError::token() const { return _token; }
    InvalidTokenError::~InvalidTokenError() throw () {}
    ExtraOperandsError::ExtraOperandsError() : RPNException("Extra operands left on stack") {}
    ExtraOperandsError::~ExtraOperandsError() throw() {}
    
    namespace {
        std::stack<int> _nsInternalS;

        void __clearStack() {
            while (!_nsInternalS.empty()) {
                _nsInternalS.pop();
            }
        }

        void __ensureOperands() {
            if (_nsInternalS.size() < 2) {
                throw StackUnderflowError();
            }
        }

        int __popOperand() {
            int ret = _nsInternalS.top();
            _nsInternalS.pop();
            return ret;
        }

        int __iadd(int a, int b) { return a + b; }
        int __isub(int a, int b) { return a - b; }
        int __imul(int a, int b) { return a * b; }
        int __idiv(int a, int b) {
            if (b == 0) throw DivisionByZeroError();
            return a / b;
        }

        bool __isOperator(char c) {
            return (c == '+' || c == '-' ||
                    c == '*' || c == '/');
        }

        bool __isValidNumber(const std::string& token) {
            if (token.empty()) return false;
            
            const char* str = token.c_str();
            char* endptr;
            std::strtol(str, &endptr, 10);
            return (*endptr == '\0');
        }
    }

    void processExpression(std::string const & expr) {
        __clearStack();
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

            if (__isValidNumber(token)) {
                _nsInternalS.push(std::atoi(token.c_str()));
            } else if (token.length() == 1 && __isOperator(token[0])) {
                // std::pair<int, int> operands = popOperands();
                __ensureOperands();
                int b = __popOperand();
                int a = __popOperand();
                
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
