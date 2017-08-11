#include "RequestManager.h"
#include "ContactRequests.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "RESTHandler.h"
#include "StringCodec.h"
#include <QJsonArray>
#include <QTimer>
#include <iostream>

namespace Pippip {

RequestManager::RequestManager(SessionState *st, QObject *parent)
: QObject(parent),
  loaded(false),
  state(st),
  contactRequests(new ContactRequests),
  acknowledged(new ContactRequests) {

}

RequestManager::~RequestManager() {

    delete contactRequests;
    delete acknowledged;

}

void RequestManager::ackRequest(const QString &ack, long requestId) {

    EnclaveRequest req(state);
    req.setRequestType("acknowledgeRequests");
    QJsonArray ackArray;
    QJsonObject ackObj;
    ackObj["ack"] = ack;
    ackObj["requestId"] = static_cast<long long>(requestId);
    ackArray.append(ackObj);
    req.setValue("acknowledgements", ackArray);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(ackComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(ackComplete(RESTHandler*)));
    timedOut = false;
    requestCompleted = false;
    QTimer::singleShot(10000, this, SLOT(ackTimedOut()));
    handler->doPost(req);

}

void RequestManager::ackComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit ackFailed(handler->getError());
        }
        else if (response) {
            if (!loadAcknowledged(response.getJson())) {
                emit ackFailed("Invalid server response");
            }
            else {
                emit requestsAcknowledged();
            }
        }
        else {
            emit ackFailed(response.getError());
        }
    }

}

void RequestManager::ackTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        emit ackFailed("Acknowledgement timed out");
    }

}

void RequestManager::loadRequests() {

    if (!loaded) {
        EnclaveRequest req(state);
        req.setRequestType("getRequests");
        QJsonArray statusArray;
        //statusArray.append("active");
        statusArray.append("pending");
        //statusArray.append("rejected");
        req.setValue("status", statusArray);
        RESTHandler *handler = new RESTHandler(this);
        connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestLoadComplete(RESTHandler*)));
        connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestLoadComplete(RESTHandler*)));
        timedOut = false;
        requestCompleted = false;
        QTimer::singleShot(10000, this, SLOT(loadTimedOut()));
        handler->doPost(req);
    }
    else {
        emit requestsLoaded();
    }

}

bool RequestManager::loadAcknowledged(const QJsonObject &json) {

    acknowledged->clear();

    QJsonValue contactValue = json["requests"];
    if (!contactValue.isArray()) {
        return false;
    }

    QJsonArray requestsArray = contactValue.toArray();
    for (const QJsonValue& value : requestsArray) {
        if (!value.isObject()) {
            return false;
        }
        Pippip::ContactRequestIn contactRequest;
        QJsonObject reqObj = value.toObject();
        contactRequest.status = reqObj["status"].toString();
        contactRequest.requestId = reqObj["requestId"].toDouble();
        QJsonObject requestedObj = reqObj["requested"].toObject();
        contactRequest.requested.nickname = requestedObj["nickname"].toString();
        contactRequest.requested.publicId = requestedObj["publicId"].toString();
        QJsonObject requestingObj = reqObj["requesting"].toObject();
        contactRequest.requesting.nickname = requestingObj["nickname"].toString();
        contactRequest.requesting.publicId = requestingObj["publicId"].toString();
        QJsonObject rsaObj = reqObj["rsaKeys"].toObject();
        contactRequest.rsaKeys.encryptionRSA = rsaObj["encryptionRSA"].toString();
        contactRequest.rsaKeys.signingRSA = rsaObj["encryptionRSA"].toString();
        QString keyStr = reqObj["keyBlock"].toString();
        std::cout << "Key block received = " << StringCodec(keyStr) << std::endl;

        contactRequest.keyBlock = coder::ByteArray(StringCodec(keyStr), true);
        acknowledged->add(contactRequest);
    }
    return true;

}

bool RequestManager::loadRequests(const QJsonObject &json) {

    contactRequests->clear();

    QJsonValue contactValue = json["requests"];
    if (!contactValue.isArray()) {
        return false;
    }

    QJsonArray requestsArray = contactValue.toArray();
    for (const QJsonValue& value : requestsArray) {
        if (!value.isObject()) {
            return false;
        }
        Pippip::ContactRequestIn contactRequest;
        QJsonObject reqObj = value.toObject();
        contactRequest.status = reqObj["status"].toString();
        contactRequest.requestId = reqObj["requestId"].toDouble();
        QJsonObject requestedObj = reqObj["requested"].toObject();
        contactRequest.requested.nickname = requestedObj["nickname"].toString();
        contactRequest.requested.publicId = requestedObj["publicId"].toString();
        QJsonObject requestingObj = reqObj["requesting"].toObject();
        contactRequest.requesting.nickname = requestingObj["nickname"].toString();
        contactRequest.requesting.publicId = requestingObj["publicId"].toString();
        //QJsonObject rsaObj = reqObj["rsaKeys"].toObject();
        //contactRequest.rsaKeys.encryptionRSA = rsaObj["encryptionRSA"].toString();
        //contactRequest.rsaKeys.signingRSA = rsaObj["encryptionRSA"].toString();
        //QString keyStr = reqObj["keyBlock"].toString();
        //contactRequest.keyBlock = coder::ByteArray(StringCodec(keyStr), true);
        contactRequests->add(contactRequest);
    }
    return true;

}

void RequestManager::requestLoadComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit loadFailed(handler->getError());
        }
        else if (response) {
            if (!loadRequests(response.getJson())) {
                emit loadFailed("Invalid server response");
            }
            else {
                emit requestsLoaded();
            }
        }
        else {
            emit loadFailed(response.getError());
        }
    }

}

void RequestManager::loadTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        emit loadFailed("Request load timed out");
    }

}

}
