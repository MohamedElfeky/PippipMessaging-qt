/*
 * SessionState.h
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

#ifndef SESSIONSTATE_H
#define SESSIONSTATE_H

#include "AccountParameters.h"
#include <string>
#include <QObject>
#include <QString>
#include <time.h>

namespace Pippip {

class RESTHandler;

class SessionState : public QObject {
    Q_OBJECT

    public:
        explicit SessionState(QObject *parent=0);
        ~SessionState();

    private:
        SessionState(const SessionState& other);
        SessionState& operator =(const SessionState& other);

    public:
        bool expired() const;
        const AccountParameters& getAccountParameters() const { return params; }
        const QString& getauthToken() const { return authToken; }
        const QString& getError() const { return error; }
        const QString& getHandlerKey() const { return handlerKey; }
        const QString& getSessionId() const { return sessionId; }
        void setAccountParameters(const AccountParameters& other) { params = other; }
        void setAuthToken(const QString& token) { authToken = token; }
        void setHandlerKey(const QString& key) { handlerKey = key; }
        void startSession();
        void touch() { timestamp = time(0); }

    signals:
        void sessionFailed(QString error);
        void sessionStarted();

    private slots:
        void requestComplete(RESTHandler *handler);
        void requestFailed(RESTHandler *handler);

    private:
        QString sessionId;
        QString error;
        time_t timestamp;
        AccountParameters params;
        QString authToken;
        QString handlerKey;

};

}

#endif // SESSIONSTATE_H
