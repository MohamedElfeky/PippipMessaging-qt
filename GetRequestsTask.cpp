/*
 * GetRequestsTask.cpp
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

#include "GetRequestsTask.h"
#include "Constants.h"
#include "ContactRequest.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "EnclaveException.h"
#include "JsonValidator.h"
#include "StringCodec.h"
#include "ValidationException.h"
#include <QDebug>
#include <QJsonArray>

namespace Pippip {

GetRequestsTask::GetRequestsTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::GET_REQUESTS_TASK;

}

void GetRequestsTask::getRequests(const QString& req) {

    requester = req;
    request->setStringValue("requester", requester);
    doRequest(10);  // 10 second timeout.

}

void GetRequestsTask::restComplete(const QJsonObject& resp) {

    response = new EnclaveResponse(resp, state);
    if (*response) {
        try {
            QJsonArray requestsArray = response->getResponseArray("requests");
            qDebug() << "Get requests complete. Retrieved " << requestsArray.size() << " requests";
            setRequests(requestsArray);
            emit getRequestsComplete(requester);
        }
        catch (EnclaveException& e) {
            QString prefix = "Enclave error in get requests task - ";
            emit getRequestsFailed(prefix + e.what());
        }
        catch (ValidationException& e) {
            QString prefix = "Validation error in get requests task - ";
            emit getRequestsFailed(prefix + e.what());
        }
    }
    else {
        emit getRequestsFailed(response->getError());
    }

}

/**
 * @brief GetRequestsTask::restFailed
 * @param error
 */
void GetRequestsTask::restFailed(const QString &error) {

    emit getRequestsFailed(error);

}

/**
 * @brief GetRequestsTask::setRequests
 * @param requests
 */
void GetRequestsTask::setRequests(const QJsonArray &requests) {

    for (auto var : requests) {
        if (!var.isObject()) {
            throw EnclaveException("Invalid server response, not a request object");
        }
        QJsonObject reqObj = var.toObject();
        ContactRequestIn request;
        request.status = JsonValidator(reqObj, "status").getString();
        request.requestId = JsonValidator(reqObj, "requestId").getLong();
        QJsonObject requested = JsonValidator(reqObj, "requested").getObject();
        request.requested.nickname = JsonValidator(requested, "nickname").getString();
        request.requested.publicId = JsonValidator(requested, "publicId").getString();
        QJsonObject requesting = JsonValidator(reqObj, "requesting").getObject();
        request.requesting.nickname = JsonValidator(requesting, "nickname").getString();
        request.requesting.publicId = JsonValidator(requesting, "publicId").getString();
        QJsonObject rsaKeys = JsonValidator(reqObj, "rsaKeys").getObject();
        request.rsaKeys.encryptionRSA = JsonValidator(rsaKeys, "encryptionRSA").getString();
        request.rsaKeys.signingRSA = JsonValidator(rsaKeys, "signingRSA").getString();
        std::string keyBlock = StringCodec(JsonValidator(reqObj, "keyBlock").getString());
        request.keyBlock = coder::ByteArray(keyBlock, true);
        requestList.push_back(request);
    }

}

}
