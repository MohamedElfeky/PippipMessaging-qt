#ifndef VAULTEXCEPTION_H
#define VAULTEXCEPTION_H

#include <exception>
#include <string>

#define EXCEPTION_THROW_SPEC noexcept

namespace Pippip {

class VaultException  : public std::exception {

    public:
        VaultException() {}
        VaultException(const std::string& msg) : message(msg) {}
        VaultException(const VaultException& other)
                : message(other.message) {}
        ~VaultException() EXCEPTION_THROW_SPEC {}


    private:
        VaultException& operator= (const VaultException& other);

    public:
        const char *what() const EXCEPTION_THROW_SPEC { return message.c_str(); }

    private:
        std::string message;

};

}

#endif // VAULTEXCEPTION_H
