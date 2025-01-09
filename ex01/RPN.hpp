#pragma once

#include <string>
#include <exception>

namespace RPN {
    class RPNException : public std::exception {
    public:
        explicit RPNException(const char* message) : _msg(message) {}
        virtual const char* what() const throw() { return _msg; }
        virtual ~RPNException() throw() {}
    private:
        const char* _msg;
    };

    class DivisionByZeroError : public RPNException {
    public:
        DivisionByZeroError() : RPNException("Division by zero") {}
        virtual ~DivisionByZeroError() throw() {}
    };

    class StackUnderflowError : public RPNException {
    public:
        StackUnderflowError() : RPNException("Not enough operands") {}
        virtual ~StackUnderflowError() throw() {}
    };

    class InvalidTokenError : public RPNException {
    public:
        explicit InvalidTokenError(const std::string& token) 
            : RPNException("Invalid token encountered"), _token(token) {}
        const std::string& token() const { return _token; }
        virtual ~InvalidTokenError() throw() {}
    private:
        std::string _token;
    };

    class ExtraOperandsError : public RPNException {
    public:
        ExtraOperandsError() : RPNException("Extra operands left on stack") {}
        virtual ~ExtraOperandsError() throw() {}
    };

    void processExpression(std::string const & expr);
    int getResult();
}
