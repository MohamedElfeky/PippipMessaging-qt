/*
 * RequestContactTask.cpp
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

#include "RequestContactTask.h"
#include "Constants.h"
#include "ContactRequest.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "StringCodec.h"
#include "ValidationException.h"
#include <sstream>

namespace Pippip {

/**
 * @brief RequestContactTask::RequestContactTask
 * @param state
 * @param parent
 */
RequestContactTask::RequestContactTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::REQUEST_CONTACT_TASK;

}

/**
 * @brief RequestContactTask::requestContact
 * @param out
 */
void RequestContactTask::requestContact(const ContactRequestOut &out) {

    request->setStringValue("idTypes", out.idTypes);
    request->setStringValue("requestedId", out.requestedId);
    request->setStringValue("requestingId", out.requestingId);
    doRequest(10);

}

/**
 * @brief RequestContactTask::restComplete
 * @param resp
 */
void RequestContactTask::restComplete(const QJsonObject& resp) {

    response = new EnclaveResponse(resp, state);
    try {
        if (*response) {
            requestId = response->getResponseLong("requestId");
            status = response->getResponseString("status");
            emit requestContactComplete();
        }
        else {
            emit requestContactFailed(response->getError());
        }
    }
    catch (ValidationException& e) {
        std::ostringstream estr;
        estr << "Request contact failed - " << e.what();
        emit requestContactFailed(StringCodec(estr.str()));
    }

}

/**
 * @brief RequestContactTask::restFailed
 * @param error
 */
void RequestContactTask::restFailed(const QString &error) {

    emit requestContactFailed(error);

}

/**
 * @brief RequestContactTask::restTimedOut
 */
void RequestContactTask::restTimedOut() {

    emit requestContactFailed("Contact request timed out");

}

}
