#include "AckRequestsTask.h"
#include "EnclaveException.h"
#include "EnclaveResponse.h"
#include "JsonValidator.h"
#include "StringCodec.h"
#include "ValidationException.h"
#include <QJsonArray>
#include <QDebug>

namespace Pippip {

/**
 * @brief AckRequestsTask::AckRequestsTask
 * @param state
 * @param parent
 */
AckRequestsTask::AckRequestsTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

}

/**
 * @brief AckRequestsTask::acknowledgeRequests
 */
void AckRequestsTask::acknowledgeRequests() {

    if (ackList.size() > 0) {
        doRequest(20);
    }

}

/**
 * @brief AckRequestsTask::addAcknowledgement
 * @param requestId
 * @param ack
 */
void AckRequestsTask::addAcknowledgement(qint64 requestId, const QString &ack) {

    AckRequest ackRequest = { requestId, ack };
    ackList.push_back(ackRequest);

}

/**
 * @brief AckRequestsTask::clear
 */
void AckRequestsTask::clear() {

    ackList.clear();

}

/**
 * @brief AckRequestsTask::restComplete
 * @param resp
 */
void AckRequestsTask::restComplete(const QJsonObject &resp) {

    response = new EnclaveResponse(resp, state);
    if (*response) {
        try {
            QJsonArray requestsArray = response->getResponseArray("requests");
            qDebug() << "Get requests complete. Retrieved " << requestsArray.size() << " requests";
            setRequests(requestsArray);
            emit ackRequestsComplete();
        }
        catch (EnclaveException& e) {
            QString prefix = "Enclave error in get requests task - ";
            emit ackRequestsFailed(prefix + e.what());
        }
        catch (ValidationException& e) {
            QString prefix = "Validation error in get requests task - ";
            emit ackRequestsFailed(prefix + e.what());
        }
    }
    else {
        emit ackRequestsFailed(response->getError());
    }

}

/**
 * @brief AckRequestsTask::restFailed
 * @param error
 */
void AckRequestsTask::restFailed(const QString &error) {

    emit ackRequestsFailed(error);

}

/**
 * @brief AckRequestsTask::setRequests
 * @param requests
 */
void AckRequestsTask::setRequests(const QJsonArray &requests) {

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
