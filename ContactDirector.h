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

#include "Nickname.h"
#include <QObject>

namespace Pippip {

class SessionState;
class ContactsDatabase;
class ContactManager;
class GetRequestsTask;
class AckRequestsTask;

class ContactDirector : public QObject {
        Q_OBJECT

    public:
        explicit ContactDirector(SessionState *state, QObject *parent = 0);
        ~ContactDirector();

    signals:

    public slots:
        void ackRequestsComplete();
        void ackRequestsFailed(const QString& error);
        void getRequests();
        void getRequestsComplete(const QString& requester);
        void getRequestsFailed(const QString& error);
        void requestTimedOut();

    public:
        void createDatabase(const QString& accountName);
        void end();
        ContactManager *getContactManager() { return contactManager; }
        const NicknameList& getNicknames() const { return nicknames; }
        void getRequests(const QString& requester);
        void openDatabase(const QString& accountName);
        void setNicknames(const NicknameList& nicks) { nicknames = nicks; }
        void start();

    private:
        SessionState *state;
        ContactsDatabase *contactsDb;
        ContactManager *contactManager;
        GetRequestsTask *getRequestsTask;
        AckRequestsTask *ackRequestsTask;
        NicknameList nicknames;

};

}

#endif // CONTACTDIRECTOR_H
