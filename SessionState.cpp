/*
 * SessionState.cpp
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

#include "SessionState.h"
#include "RESTHandler.h"

namespace Pippip {

static const time_t SESSION_TTL = 1800; // 30 minutes

SessionState::SessionState(QObject *parent)
: QObject(parent) {

    setObjectName("SessionState");

}

SessionState::~SessionState() {
}

bool SessionState::expired() const {

    time_t now = time(0);
    return now - timestamp >= SESSION_TTL;

}

void SessionState::requestComplete(RESTHandler *handler) {

    QJsonValue value = handler->getResponse()["error"];
    if (value != QJsonValue::Undefined) {
        error = value.toString();
        emit sessionFailed(error);
    }
    else {
        value = handler->getResponse()["sessionId"];
        if (value == QJsonValue::Undefined) {
            error = "Invalid server response";
            emit sessionFailed(error);
        }
        else {
            sessionId = value.toString();
            touch();
            error = "";
            emit sessionStarted();
        }
    }
    handler->deleteLater();

}

void SessionState::requestFailed(RESTHandler *handler) {

    error = handler->getError();
    emit sessionFailed(error);
    handler->deleteLater();

}

/*void SessionState::setAccountParameters(const AccountParameters &otherparams) {

    params = otherparams;

}*/

void SessionState::startSession() {

    error = "Request timed out";

    RESTHandler *handler = new RESTHandler;
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestFailed(RESTHandler*)));
    handler->doGet(QString("https://pippip.io:2880/session/"));


}

}
