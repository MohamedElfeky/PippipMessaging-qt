/*
 * StringCodec.cpp
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

#include "StringCodec.h"
#include <QByteArray>
#include <string.h>

namespace Pippip {

StringCodec::StringCodec(const QString& q)
: qtString(q),
  strLength(q.length()) {

    QByteArray data = qtString.toUtf8();
    cstr = new char[strLength + 1];
    memset(cstr, 0, strLength + 1);
    strncpy(cstr, data.constData(), strLength);
    stdString = std::string(cstr);

}

StringCodec::StringCodec(const std::string& c)
: stdString(c),
  strLength(c.length()) {

    cstr = new char[strLength + 1];
    strcpy(cstr, stdString.c_str());
    qtString = QString(cstr);

}

StringCodec::StringCodec(const char *str)
: qtString(str),
  stdString(str) {

    cstr = new char[strlen(str) + 1];
    strcpy(cstr, str);

}

StringCodec::~StringCodec() {

    delete[] cstr;

}

}
