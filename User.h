/*
 * User.h
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

#ifndef USER_H
#define USER_H

namespace Pippip {

class AccountParameters;
class MessageDatabase;
class Contacts;

class User {

    public:
        User();
        ~User();

    private:
        User(const User& other);
        User& operator =(const User& other);

    private:
        AccountParameters *params;
        MessageDatabase *messages;
        Contacts *contacts;

};

}

#endif // USER_H

