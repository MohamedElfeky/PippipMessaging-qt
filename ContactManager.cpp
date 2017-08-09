#include "ContactManager.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "ContactRequest.h"
#include "RESTHandler.h"
#include "SessionState.h"
#include "Contacts.h"
#include "Message.h"
#include "Function.h"
#include "ByteCodec.h"
#include "StringCodec.h"
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>

namespace Pippip {

ContactManager::ContactManager(SessionState *st, QObject *parent)
: QObject(parent),
  state(st),
  contacts(new Contacts) {

}

ContactManager::~ContactManager() {

    delete contacts;

}

void ContactManager::addComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit addFailed(handler->getError());
        }
        else if (response) {
            if (!loadContacts(response.getJson())) {
                emit addFailed("Invalid server response");
            }
            else if (toDelete.size() > 0) {
                deleteContacts();
            }
        }
        else {
            emit addFailed(response.getError());
        }
    }

}

void ContactManager::addContact(const Contact &contact) {

    toAdd.clear();
    toAdd.push_back(contact);
    addContacts();

}

void ContactManager::addContacts() {

    EnclaveRequest req(state);
    req.setRequestType("addContacts");
    QJsonArray contactArray;
    encodeContacts(toAdd, contactArray);
    req.setValue("contacts", contactArray);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(addComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(addComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(addTimedOut()));
    handler->doPost(req);

}

void ContactManager::addTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        emit addFailed("Contact request timed out");
    }

}

void ContactManager::contactLoadComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit loadFailed(handler->getError());
        }
        else if (response) {
            if (!loadContacts(response.getJson())) {
                emit loadFailed("Invalid server response");
            }
            else {
                emit contactsLoaded();
            }
        }
        else {
            emit loadFailed(response.getError());
        }
    }

}

void ContactManager::contactRequestComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit contactRequestFailed(handler->getError());
        }
        else if (response) {
            long requestId = response.getValue("requestId").toDouble();
            QString status = response.getValue("status").toString();
            emit contactRequestComplete(requestId, status);
        }
        else {
            emit contactRequestFailed(response.getError());
        }
    }

}

void ContactManager::deleteComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit deleteFailed(handler->getError());
        }
        else if (response) {
            if (!loadContacts(response.getJson())) {
                emit deleteFailed("Invalid server response");
            }
        }
        else {
            emit deleteFailed(response.getError());
        }
    }

}

void ContactManager::deleteContacts() {

    EnclaveRequest req(state);
    req.setRequestType("deleteContacts");
    QJsonArray contactArray;
    encodeContacts(toDelete, contactArray);
    req.setValue("contacts", contactArray);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(deleteComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(deleteComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(deleteTimedOut()));
    handler->doPost(req);

}

void ContactManager::deleteTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        emit deleteFailed("Contact request timed out");
    }

}

void ContactManager::encodeContacts(const ContactList &list, QJsonArray &array) {

    try {
        for (auto contact : list) {
            QJsonObject contactObj;
            contactObj["contactId"] = static_cast<int>(contact.contactId);
            contactObj["status"] = contact.status;
            CK::GCMCodec codec;
            codec << contact;
            codec.encrypt(state->contactKey, state->authData);
            StringCodec encoded(codec.toArray().toHexString());
            contactObj["encoded"] = encoded.getQtString();
            array.append(contactObj);
        }
    }
    catch (CK::EncodingException& e) {
        // Really not sure what to do here.
        StringCodec error(e.what());
        qDebug() << "GCM error during contact list encoding - " << error.getQtString();
    }

}

void ContactManager::loadContacts() {

    EnclaveRequest req(state);
    req.setRequestType("getContacts");
    QJsonArray statusArray;
    statusArray.append("active");
    statusArray.append("pending");
    statusArray.append("rejected");
    req.setValue("status", statusArray);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(contactLoadComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(contactLoadComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(loadTimedOut()));
    handler->doPost(req);

}

bool ContactManager::loadContacts(const QJsonObject &json) {

    contacts->clear();

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
        contact.contactId = contactObj["contactId"].toDouble();
        QString encodedStr = contactObj["encoded"].toString();
        coder::ByteArray encoded(StringCodec(encodedStr), true);
        CK::GCMCodec codec(encoded);
        codec.decrypt(state->contactKey, state->authData);
        codec >> contact;
        contacts->add(contact);
    }
    return true;

}

void ContactManager::loadTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        emit loadFailed("Contact load timed out");
    }

}

void ContactManager::reconcile(const Contacts &record) {

    record.diff(*contacts, toAdd, toDelete);
    if (toAdd.size() > 0) {
        addContacts();
    }
    else if (toDelete.size() > 0) {
        deleteContacts();
    }

}

void ContactManager::requestContact(const ContactRequestOut &request) {

    EnclaveRequest req(state);
    req.setRequestType("requestContact");
    req.setValue("idTypes", request.idTypes);
    req.setValue("requestedId", request.requestedId);
    req.setValue("requestingId", request.requestingId);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(contactRequestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(contactRequestComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(contactRequestTimedOut()));
    handler->doPost(req);

}

void ContactManager::contactRequestTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        emit contactRequestFailed("Contact request timed out");
    }

}

}
