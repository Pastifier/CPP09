#pragma once

#include <string>
#include <exception>

namespace RPN {
    class RPNException : public std::exception {
    public:
        explicit RPNException(const char* message);
        virtual const char* what() const throw();
        virtual ~RPNException() throw();
    private:
        const char* _msg;
    };

    class DivisionByZeroError : public RPNException {
    public:
        DivisionByZeroError();
        virtual ~DivisionByZeroError() throw();
    };

    class StackUnderflowError : public RPNException {
    public:
        StackUnderflowError();
        virtual ~StackUnderflowError() throw();
    };

    class InvalidTokenError : public RPNException {
    public:
        InvalidTokenError();
        // const std::string& token() const;
        virtual ~InvalidTokenError() throw();
    };

    class ExtraOperandsError : public RPNException {
    public:
        ExtraOperandsError();
        virtual ~ExtraOperandsError() throw();
    };

    void processExpression(std::string const & expr);
    int getResult();
}
