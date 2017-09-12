/*
 * ContactDirector.h
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

#ifndef CONTACTDIRECTOR_H
#define CONTACTDIRECTOR_H

#include <QObject>

namespace Pippip {

class SessionState;
class ContactsDatabase;
class ContactManager;
class GetRequestsTask;

class ContactDirector : public QObject {
        Q_OBJECT

    public:
        explicit ContactDirector(SessionState *state, QObject *parent = 0);
        ~ContactDirector();

    signals:

    public slots:
        void getRequests();
        void getRequestsComplete();
        void getRequestsFailed(const QString& error);

    public:
        void createDatabase(const QString& accountName);
        void end();
        void openDatabase(const QString& accountName);
        void start();

    private:
        SessionState *state;
        ContactsDatabase *contactsDb;
        ContactManager *contactManager;
        GetRequestsTask *getRequestsTask;

};

}

#endif // CONTACTDIRECTOR_H
