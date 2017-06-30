#include "ContactManager.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "ContactRequest.h"
#include "RESTHandler.h"
#include "SessionState.h"
#include "CriticalAlert.h"
#include <QTimer>
#include <QJsonArray>

namespace Pippip {

ContactManager::ContactManager(QObject *parent, SessionState *st)
: QObject(parent),
  state(st) {

}

void ContactManager::contactLoadComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Contact Load Failed", "Unable to fetch contacts", handler->getError());
            alert.exec();
        }
        else if (response && loadContacts(response.getJson())) {
            emit contactsLoaded();
        }
        else {
            CriticalAlert alert("Contact Load Failed", "Unable to fetch contacts", response.getError());
            alert.exec();
        }
    }

}

void ContactManager::contactRequestComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Contact Request Failed", "Unable to request contact", handler->getError());
            alert.exec();
        }
        else if (response) {
            QJsonObject contactObj = response.getJson();
            Pippip::Contact contact;
            QJsonArray requests = contactObj["requests"].toArray();
            QJsonObject request = requests[0].toObject();
            contact.status = request["status"].toString();
            QJsonObject requested = request["requested"].toObject();
            contact.entity.nickname = requested["nickname"].toString();
            contact.entity.publicId = requested["publicId"].toString();
            emit requestedContact(contact);
        }
        else {
            CriticalAlert alert("Contact Request Failed", "Unable to request contact", response.getError());
            alert.exec();
        }
    }

}

void ContactManager::loadContacts() {

    EnclaveRequest req(state);
    req.setRequestType("getContacts");
    req.setValue("publicId", state->publicId);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(contactLoadComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(contactLoadComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

bool ContactManager::loadContacts(const QJsonObject &json) {

    contacts.clear();

    QJsonValue contactValue = json["contacts"];
    if (!contactValue.isArray()) {
        CriticalAlert alert("Fetch Contacts Error", "Invalid server response");
        alert.exec();
        return false;
    }

    QJsonArray contactsJson = contactValue.toArray();
    for (const QJsonValue& value : contactsJson) {
        if (!value.isObject()) {
            CriticalAlert alert("Fetch Contacts Error", "Invalid server response");
            alert.exec();
            return false;
        }
        Pippip::Contact contact;
        QJsonObject contactObj = value.toObject();
        contact.status = contactObj["status"].toString();
        QJsonObject entity = contactObj["entity"].toObject();
        contact.entity.nickname = entity["nickname"].toString();
        contact.entity.publicId = entity["publicId"].toString();
        contact.entity.encryptionRSA = entity["encryptionRSA"].toString();
        contact.entity.signingRSA = entity["signingRSA"].toString();
        QJsonArray messageKeys = contactObj["messageKeys"].toArray();
        for (QJsonValue key : messageKeys) {
            contact.messageKeys.append(key.toString());
        }
        contacts.push_back(contact);
    }
    return true;

}

void ContactManager::loadRequests() {

    EnclaveRequest req(state);
    req.setRequestType("getRequests");
    req.setValue("publicId", state->publicId);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestLoadComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestLoadComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

void ContactManager::requestContact(const ContactRequest &request) {

    EnclaveRequest req(state);
    req.setRequestType("requestContact");
    req.setValue("publicId", state->publicId);
    QJsonObject requestObj;
    QJsonObject requestingObj;
    requestingObj["nickname"] = request.requesting.nickname;
    requestingObj["publicId"] = request.requesting.publicId;
    requestObj["requesting"] = requestingObj;
    QJsonObject requestedObj;
    requestedObj["nickname"] = request.requested.nickname;
    requestedObj["publicId"] = request.requested.publicId;
    requestObj["requested"] = requestedObj;
    requestObj["privateRequest"] = request.privateRequest;
    QJsonArray requests;
    requests.append(requestObj);
    req.setValue("requests", requests);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(contactRequestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(contactRequestComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

void ContactManager::requestLoadComplete(RESTHandler *) {

}

void ContactManager::requestTimedOut() {

    if (!requestComplete) {
        timedOut = true;
        CriticalAlert alert("Enclave Request Error", "Contact request timed out");
        alert.exec();
    }

}

}
