/*
 * TaskException.h
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

#ifndef TASKEXCEPTION_H
#define TASKEXCEPTION_H

#include <exception>
#include <string>

#define EXCEPTION_THROW_SPEC noexcept

namespace Pippip {

class TaskException  : public std::exception {

    public:
        TaskException() {}
        TaskException(const std::string& msg) : message(msg) {}
        TaskException(const TaskException& other)
                : message(other.message) {}
        ~TaskException() EXCEPTION_THROW_SPEC {}


    private:
        TaskException& operator= (const TaskException& other);

    public:
        const char *what() const EXCEPTION_THROW_SPEC { return message.c_str(); }

    private:
        std::string message;

};

}

#endif // TASKEXCEPTION_H
