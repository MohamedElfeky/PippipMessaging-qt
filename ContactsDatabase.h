/*
 * ContactsDatabase
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

#ifndef CONTACTSDATABASE_H
#define CONTACTSDATABASE_H

#include "Contact.h"

class QString;

namespace Pippip {

class SessionState;

class ContactsDatabase {

    protected:
        ContactsDatabase() {}

    public:
        virtual ~ContactsDatabase() {}

    public:
        virtual void addContact(const Contact& contact, SessionState *state)=0;
        virtual void close()=0;
        virtual void create(const QString& account)=0;
        virtual void getContacts(ContactList& list, SessionState *state)=0;
        virtual int getNextContactId()=0;
        virtual void open(const QString& account)=0;
        virtual void updateContact(const Contact& contact, SessionState *state)=0;

};

}

#endif // CONTACTSDATABASE_H
