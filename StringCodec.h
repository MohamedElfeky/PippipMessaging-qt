/*
 * StringCodec.h
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

#ifndef STRINGCODEC_H
#define STRINGCODEC_H

#include <QString>
#include <string>

namespace Pippip {

class StringCodec {

    public:
        StringCodec(const QString& qtString);
        StringCodec(const std::string& stdString);
        explicit StringCodec(const char *str);   // Null terminated string
        ~StringCodec();

    public:
        explicit operator const char* () const { return cstr; }
        operator const QString& () const { return qtString; }
        operator const std::string& () const { return stdString; }

    public:
        const std::string& getStdString() const { return stdString; }
        const QString& getQtString() const { return qtString; }
        size_t length() const { return strLength; }

    private:
        QString qtString;
        std::string stdString;
        // bool qt;
        char* cstr;
        size_t strLength;

};

}

#endif // STRINGCODEC_H
