#ifndef DATABASEEXCEPTION_H
#define DATABASEEXCEPTION_H

#include <exception>
#include <string>

#define EXCEPTION_THROW_SPEC noexcept

namespace Pippip {

class DatabaseException  : public std::exception {

    public:
        DatabaseException() {}
        DatabaseException(const std::string& msg) : message(msg) {}
        DatabaseException(const DatabaseException& other)
                : message(other.message) {}
        ~DatabaseException() EXCEPTION_THROW_SPEC {}


    private:
        DatabaseException& operator= (const DatabaseException& other);

    public:
        const char *what() const EXCEPTION_THROW_SPEC { return message.c_str(); }

    private:
        std::string message;

};

}

#endif // DATABASEEXCEPTION_H
