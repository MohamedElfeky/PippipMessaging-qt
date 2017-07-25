#include "ContactManager.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "ContactRequest.h"
#include "RESTHandler.h"
#include "SessionState.h"
#include "Message.h"
#include <QTimer>
#include <QJsonArray>
#include <iostream>

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
            emit requestFailed("Fetch Contacts", handler->getError());
        }
        else if (response) {
            if (!loadContacts(response.getJson())) {
                emit requestFailed("Fetch Contacts", "Invalid server response");
            }
            else {
                emit contactsLoaded();
            }
        }
        else {
            emit requestFailed("Fetch Contacts", response.getError());
        }
    }

}

void ContactManager::contactRequestComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed("Contact Request", handler->getError());
        }
        else if (response) {
            QJsonObject contactObj = response.getJson();
            Pippip::Contact contact;
            QJsonArray requests = contactObj["requests"].toArray();
            QJsonObject request = requests[0].toObject();
            contact.status = request["status"].toString();
            QJsonObject requesting = request["requesting"].toObject();
            contact.contactOf = requesting["nickname"].toString();
            QJsonObject requested = request["requested"].toObject();
            contact.entity.nickname = requested["nickname"].toString();
            contact.entity.publicId = requested["publicId"].toString();
            contact.currentKey = 0;
            contacts.push_back(contact);
            QString name;
            if (contact.entity.nickname.length() > 0) {
                name = contact.entity.nickname;
            }
            else {
                name = contact.entity.publicId;
            }
            emit contactRequested(name);
        }
        else {
            emit requestFailed("Contact Request", response.getError());
        }
    }

}

bool ContactManager::getContactByNickname(const QString &nickname, Contact& aContact) const {

    for(auto contact : contacts) {
        if (contact.entity.nickname == nickname) {
            aContact = contact;
            return true;
        }
    }
    return false;

}

bool ContactManager::getContactById(const QString &id, Contact& aContact) const {

    for(auto contact : contacts) {
        if (contact.entity.publicId == id) {
            aContact = contact;
            return true;
        }
    }
    return false;

}

void ContactManager::incrementSequences(const Message &message) {

    for (auto contact : contacts) {
        if ((message.sender == contact.contactOf || message.senderId == state->publicId)
                && message.recipient == contact.entity.nickname
                && message.recipientId == contact.entity.publicId) {
            contact.currentKey = message.keyIndex + 1;
            if (contact.currentKey > 9) {
                contact.currentKey = 0;
            }
            contact.currentSequence = message.sequence + 1;
        }
    }

}

void ContactManager::loadContacts() {

    emit contactsLoaded();

/*    EnclaveRequest req(state);
    req.setRequestType("getContacts");
    req.setValue("publicId", state->publicId);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(contactLoadComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(contactLoadComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req); */

}

bool ContactManager::loadContacts(const QJsonObject &json) {

    contacts.clear();

    QJsonValue contactValue = json["contacts"];
    if (!contactValue.isArray()) {
        return false;
    }

    QJsonArray contactsJson = contactValue.toArray();
    for (const QJsonValue& value : contactsJson) {
        if (!value.isObject()) {
            return false;
        }
        Pippip::Contact contact;
        QJsonObject contactObj = value.toObject();
        contact.status = contactObj["status"].toString();
        contact.contactOf = contactObj["contactOf"].toString();
        QJsonObject entity = contactObj["entity"].toObject();
        contact.entity.nickname = entity["nickname"].toString();
        contact.entity.publicId = entity["publicId"].toString();
        contact.rsaKeys.encryptionRSA = entity["encryptionRSA"].toString();
        contact.rsaKeys.signingRSA = entity["signingRSA"].toString();
        contact.nonce = contactObj["nonce"].toString();
        contact.authData = contactObj["authData"].toString();
        QJsonArray messageKeys = contactObj["messageKeys"].toArray();
        for (QJsonValue key : messageKeys) {
            contact.messageKeys.append(key.toString());
        }
        contacts.push_back(contact);
    }
    return true;

}

void ContactManager::loadRequests() {

    std::cout << "loadRequests called" << std::endl;

/*
    EnclaveRequest req(state);
    req.setRequestType("getRequests");
    req.setValue("publicId", state->publicId);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestLoadComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestLoadComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);
*/
}

void ContactManager::requestContact(const ContactRequest &request) {

    EnclaveRequest req(state);
    req.setRequestType("requestContact");
    req.setValue("requestType", request.requestType);
    req.setValue("requestingId", request.requestingId);
    req.setValue("requestedId", request.requestedId);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(contactRequestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(contactRequestComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

void ContactManager::requestLoadComplete(RESTHandler *) {

}

void ContactManager::requestTimedOut() {

    if (!requestComplete) {
        timedOut = true;
        emit requestFailed("Enclave Request", "Contact request timed out");
    }

}

}
