#ifndef SESSIONEXCEPTION_H
#define SESSIONEXCEPTION_H

#include <exception>
#include <string>

#define EXCEPTION_THROW_SPEC noexcept

namespace Pippip {

class SessionException  : public std::exception {

    public:
        SessionException() {}
        SessionException(const std::string& msg) : message(msg) {}
        SessionException(const SessionException& other)
                : message(other.message) {}
        ~SessionException() {}

    private:
        SessionException& operator= (const SessionException& other);

    public:
        const char *what() const EXCEPTION_THROW_SPEC { return message.c_str(); }

    private:
        std::string message;

};

}

#endif // SESSIONEXCEPTION_H
