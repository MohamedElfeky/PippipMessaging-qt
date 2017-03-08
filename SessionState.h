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

#include <string>
#include <QObject>

class RESTHandler;

namespace Pippip {

class SessionState : public QObject {
    Q_OBJECT

    public:
        explicit SessionState(QObject *parent=0);
        ~SessionState();

    private:
        SessionState(const SessionState& other);
        SessionState& operator =(const SessionState& other);

    public:
        const std::string& getError() const { return error; }
        const std::string& getSessionId() const { return sessionId; }
        bool startSession();

    private:
        static void requestSession(SessionState *state);

    private slots:
        void requestComplete(RESTHandler *handler);

    private:
        std::string sessionId;
        std::string error;

};

}

#endif // SESSIONSTATE_H
