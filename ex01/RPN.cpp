#include "RPN.hpp"
#include <stack>
#include <cstring>
#include <sstream>
#include <cstdlib>

namespace RPN {

    RPNException::RPNException(const char* message) : _msg(message) {}
    RPNException::~RPNException() throw() {}
    const char* RPNException::what() const throw()  { return _msg; }

    DivisionByZeroError::DivisionByZeroError() : RPNException("Division by zero") {}
    DivisionByZeroError::~DivisionByZeroError() throw() {}
    StackUnderflowError::StackUnderflowError() : RPNException("Not enough operands") {}
    StackUnderflowError::~StackUnderflowError() throw() {}
    InvalidTokenError::InvalidTokenError() 
        : RPNException("Invalid token encountered") {}
    // const std::string& InvalidTokenError::token() const { return _token; }
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

    }

    void processExpression(std::string const & expr) {
        __clearStack();
        std::istringstream iss(expr);
        std::string token;

        while (iss >> token) {
            if (token.empty()) continue;

            if (token.length() == 1 && __isOperator(token[0])) {
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
                std::istringstream numStream(token);
                int value;
                if (!(numStream >> value) || !(numStream >> std::ws).eof()) {
                    throw InvalidTokenError();
                }
                _nsInternalS.push(value);
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
