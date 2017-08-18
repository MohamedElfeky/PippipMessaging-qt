/*
 * EnclaveRequestTask.cpp
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

#include "EnclaveRequestTask.h"
#include "EnclaveResponse.h"
#include "EnclaveException.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include <QTimer>

namespace Pippip {

EnclaveRequestTask::EnclaveRequestTask(SessionState *st, QObject *parent)
: QObject(parent),
  request(st),
  response(0),
  state(st) {
}

EnclaveRequestTask::~EnclaveRequestTask() {

    delete response;

}

void EnclaveRequestTask::doRequest() {

    if (state->sessionState != SessionState::authenticated) {
        throw EnclaveException("Session not authenticated");
    }

    requestCompleted = timedOut = false;

    request.setRequestType(taskName);
    RESTHandler *handler = new RESTHandler;
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(request);

}

void EnclaveRequestTask::requestComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        response = new EnclaveResponse(handler->getResponse(), state);
        if (!handler->successful()) {
            error = handler->getError();
            emit requestFailed(this);
        }
        else if (*response) {
            if (requestComplete()) {
                emit requestComplete(this);
            }
            else {
                emit requestFailed(this);
            }
        }
        else {
            error = response->getError();
            emit requestFailed(this);
        }
    }
    handler->deleteLater();

}

void EnclaveRequestTask::requestTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        error = "Request timed out";
        emit requestFailed(this);
    }

}

}
