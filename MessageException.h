#ifndef MESSAGEEXCEPTION_H
#define MESSAGEEXCEPTION_H

#include <exception>
#include <string>

#define EXCEPTION_THROW_SPEC noexcept

namespace Pippip {

class MessageException  : public std::exception {

    public:
        MessageException() {}
        MessageException(const std::string& msg) : message(msg) {}
        MessageException(const MessageException& other)
                : message(other.message) {}
        ~MessageException() EXCEPTION_THROW_SPEC {}


    private:
        MessageException& operator= (const MessageException& other);

    public:
        const char *what() const EXCEPTION_THROW_SPEC { return message.c_str(); }

    private:
        std::string message;

};

}

#endif // MESSAGEEXCEPTION_H
