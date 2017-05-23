/*
 * EnclaveRequester.cpp
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

#include "EnclaveRequester.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "EnclaveException.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include <QTimer>

namespace Pippip {

EnclaveRequester::EnclaveRequester(SessionState *state, QObject *parent)
: SessionTask(parent, state) {
}

EnclaveRequester::~EnclaveRequester() {
}

void EnclaveRequester::request(EnclaveRequest &request) {

    if (state->sessionState != SessionState::authenticated) {
        throw EnclaveException("Session not authenticated");
    }

    requestCompleted = false;

    RESTHandler *handler = new RESTHandler;
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(request);

}

void EnclaveRequester::requestComplete(RESTHandler *handler) {

    if (!requestCompleted) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed(handler->getError());
        }
        else if (response) {
            emit requestComplete(response);
        }
        else {
            emit requestFailed(response.getError());
        }
    }
    handler->deleteLater();

}

void EnclaveRequester::requestTimedOut() {

    if (!requestCompleted) {
        requestCompleted = true;
        emit requestFailed("Request timed out");
    }

}

}
