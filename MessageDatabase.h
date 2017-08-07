/*
 * MessageDatabase.h
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

#ifndef MESSAGEDATABASE_H
#define MESSAGEDATABASE_H

#include "Message.h"
#include <QSqlDatabase>

namespace Pippip {

class MessageDatabase {

    public:
        MessageDatabase();
        ~MessageDatabase();

    private:
        MessageDatabase(const MessageDatabase& other);
        MessageDatabase& operator =(const MessageDatabase& other);

    public:
        bool addMessage(const Message& message);
        void close();
        static bool create(const QString& account);
        const QString& getLastError() const { return lastError; }
        static void initialize(/* SessionState *state */);    // Throws DatabaseException
        bool open(const QString& account);

    private:
        QSqlDatabase database;
        QString lastError;

};

}

#endif // MESSAGEDATABASE_H

