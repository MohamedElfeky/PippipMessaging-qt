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
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "EnclaveException.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include <QTimer>

namespace Pippip {

/**
 * @brief EnclaveRequestTask::EnclaveRequestTask
 * @param st
 * @param parent
 */
EnclaveRequestTask::EnclaveRequestTask(SessionState *st, QObject *parent)
: RESTTask(parent),
  request(new EnclaveRequest(st)),
  response(0),
  state(st) {
}

/**
 * @brief EnclaveRequestTask::~EnclaveRequestTask
 */
EnclaveRequestTask::~EnclaveRequestTask() {

    delete response;
    delete request;

}

/**
 * @brief EnclaveRequestTask::doRequest
 * @param timeout
 */
void EnclaveRequestTask::doRequest(int timeout) {

    if (state->sessionState != SessionState::authenticated) {
        throw EnclaveException("Session not authenticated");
    }

    request->setRequestType(taskName);
    doRESTPost(timeout, *request);

}

/**
 * @brief EnclaveRequestTask::restFailed
 * @param error
 */
void EnclaveRequestTask::restFailed(const QString &error) {

    emit enclaveRequestFailed(error);

}

/**
 * @brief EnclaveRequestTask::restTimedOut
 */
void EnclaveRequestTask::restTimedOut() {

    emit enclaveRequestTimedOut();

}

}
