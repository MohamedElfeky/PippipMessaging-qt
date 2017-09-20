#ifndef SESSIONEXCEPTION_H
#define SESSIONEXCEPTION_H

#include <exception>
#include <string>

#define EXCEPTION_THROW_SPEC noexcept

namespace Pippip {

class xSessionException  : public std::exception {

    public:
        xSessionException() {}
        xSessionException(const std::string& msg) : message(msg) {}
        xSessionException(const SessionException& other)
                : message(other.message) {}
        ~xSessionException() {}

    private:
        xSessionException& operator= (const xSessionException& other);

    public:
        const char *what() const EXCEPTION_THROW_SPEC { return message.c_str(); }

    private:
        std::string message;

};

}

#endif // SESSIONEXCEPTION_H
