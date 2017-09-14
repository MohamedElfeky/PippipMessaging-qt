/*
 * ValidationException.h
 * Copyright (C) 2017 Steve Brenneis <steve.brenneis@secomm.org>
 *
 * PippipMessaging is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PippipMessaging is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VALIDATIONEXCEPTION_H
#define VALIDATIONEXCEPTION_H

#include <exception>
#include <string>

#define EXCEPTION_THROW_SPEC noexcept

namespace Pippip {

class ValidationException  : public std::exception {

    public:
        ValidationException() {}
        ValidationException(const std::string& msg) : message(msg) {}
        ValidationException(const ValidationException& other)
                : message(other.message) {}
        ~ValidationException() EXCEPTION_THROW_SPEC {}


    private:
        ValidationException& operator= (const ValidationException& other);

    public:
        const char *what() const EXCEPTION_THROW_SPEC { return message.c_str(); }

    private:
        std::string message;

};

}

#endif // VALIDATIONEXCEPTION_H
