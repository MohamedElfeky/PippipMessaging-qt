#include "RequestContactTask.h"
#include "Constants.h"
#include "EnclaveResponse.h"
#include "ContactRequest.h"
#include "SessionState.h"
#include "ContactsDatabase.h"
#include "StringCodec.h"
#include "ByteCodec.h"
#include "Function.h"
#include "Contacts.h"
#include "DatabaseException.h"
#include <QJsonValue>
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>

namespace Pippip {

RequestContactTask::RequestContactTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::REQUEST_CONTACT_TASK;

}

void RequestContactTask::buildAndStoreContact() {

    Contact contact;
    contact.status = status;
    contact.requestId = requestId;

    if (requestOut.idTypes == Constants::NICKNAME_NICKNAME) {
        contact.contactOf = requestOut.requestingId;
        contact.entity.nickname = requestOut.requestedId;
    }
    else if (requestOut.idTypes == Constants::NICKNAME_PUBLICID) {
        contact.contactOf = requestOut.requestingId;
        contact.entity.publicId = requestOut.requestedId;
    }
    else if (requestOut.idTypes == Constants::PUBLICID_NICKNAME) {
        contact.entity.nickname = requestOut.requestedId;
    }
    else if (requestOut.idTypes == Constants::PUBLICID_PUBLICID) {
        contact.entity.publicId = requestOut.requestedId;
    }

    CK::GCMCodec codec;
    codec << contact;
    codec.encrypt(state->contactKey, state->authData);
    DatabaseContact databaseContact;
    databaseContact.id = 0;
    databaseContact.contact = ByteCodec(codec.toArray());

    ContactsDatabase *database = ContactsDatabase::open(state);
    database->addContact(databaseContact);
    database->close();

    state->contacts->add(contact);
    serverContact.contactId = databaseContact.id;
    serverContact.contact = databaseContact.contact;
    serverContact.status = contact.status;

}

bool RequestContactTask::requestComplete() {

    QJsonValue idValue = response->getValue("requestId");
    if (!idValue.isDouble()) {
        error = "Invalid server response";
        return false;
    }
    requestId = idValue.toDouble();

    QJsonValue statusValue = response->getValue("status");
    if (!statusValue.isString()) {
        error = "Invalid server response";
        return false;
    }
    status = statusValue.toString();

    try {
        buildAndStoreContact();
    }
    catch (CK::EncodingException& e) {
        error = "Encoding error - " + StringCodec(e.what());
        return false;
    }
    catch (DatabaseException& e) {
        error = "Database error - " + StringCodec(e.what());
        return false;
    }

    return true;

}

void RequestContactTask::setRequest(const ContactRequestOut &out) {

    requestOut = out;
    request.setStringValue("idTypes", out.idTypes);
    request.setStringValue("requestedId", out.requestedId);
    request.setStringValue("requestingId", out.requestingId);

}

}
