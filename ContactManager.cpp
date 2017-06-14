#include "ContactManager.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
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

void ContactManager::addContact(const Contact &contact) {

    EnclaveRequest req(state);
    req.setRequestType("addContact");
    req.setValue("publicId", state->publicId);
    QJsonObject contactObj;
    contactObj["nickname"] = contact.nickname;
    contactObj["publicId"] = contact.publicId;
    QJsonArray contacts;
    contacts.append(contactObj);
    req.setValue("contacts", contacts);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(contactAdded(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(contactAdded(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

void ContactManager::contactAdded(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            CriticalAlert alert("Contact Load Failed", "Unable to fetch contacts", handler->getError());
            alert.exec();
        }
        else if (response) {
            QJsonObject contactObj = response.getJson();
            Pippip::Contact contact;
            contact.status = contactObj["status"].toString();
            contact.nickname = contactObj["nickname"].toString();
            contact.publicId = contactObj["publicId"].toString();
            emit addedContact(contact);
        }
        else {
            CriticalAlert alert("Contact Load Failed", "Unable to fetch contacts", response.getError());
            alert.exec();
        }
    }

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
        contact.nickname = contactObj["nickname"].toString();
        contact.publicId = contactObj["publicId"].toString();
        contact.encryptionRSA = contactObj["encryptionRSA"].toString();
        contact.signingRSA = contactObj["signingRSA"].toString();
        QJsonArray messageKeys = contactObj["messageKeys"].toArray();
        for (QJsonValue key : messageKeys) {
            contact.keys.append(key.toString());
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
