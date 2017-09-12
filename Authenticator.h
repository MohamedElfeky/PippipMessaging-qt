/*
 * Authenticator.h
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

#ifndef PIPPIP_AUTHENTICATOR_H
#define PIPPIP_AUTHENTICATOR_H

#include "SessionTask.h"

class LoginDialog;
class QString;

namespace Pippip {

class Vault;
class RESTHandler;

class Authenticator : public SessionTask {
        Q_OBJECT

    public:
        Authenticator(LoginDialog *parent, Vault *vault);
        Authenticator(SessionState *state, QObject *parent = 0);
        ~Authenticator();

    private:
        Authenticator(const Authenticator& other);
        Authenticator& operator =(const Authenticator& other);

    signals:
        void authenticationComplete(bool);
        void loginComplete(int);
        void loginReset();
        void loggedOut();
        void sessionFailed(const QString& error);
        void updateInfo(QString info);
        void updateStatus(QString);


    public slots:
        void authorizeComplete(RESTHandler*);
        void authorizeTimedOut();
        void challengeComplete(RESTHandler*);
        void challengeTimedOut();
        void logoutComplete(RESTHandler*);
        void logoutTimedOut();
        void requestComplete(RESTHandler*);
        void requestTimedOut();

    public:
        void authenticate(const QString& accountName, const QString& passphrase);
        void logOut();
        void reset();

    protected:
        void sessionComplete(const QString& result);

    private:
        void authFailed(const QString& error);
        void doAuthorized();
        void doChallenge();
        void doLogin();

    private:
        bool responseComplete;
        bool challengeCompleted;
        bool authComplete;
        bool logoutCompleted;
        bool timedOut;
        LoginDialog *dialog;
        Vault *vault;

};

}

#endif // PIPPIP_AUTHENTICATOR_H
