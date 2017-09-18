#include "ContactManager.h"
#include "ContactRequest.h"
#include "Contact.h"
#include "ContactsDatabase.h"
#include "Constants.h"
#include "SessionState.h"
#include "ByteCodec.h"
#include "StringCodec.h"
#include "StatusController.h"
#include "DatabaseException.h"
#include <QDebug>
#include <CryptoKitty-C/encoding/GCMCodec.h>

namespace Pippip {

/**
 * @brief ContactManager::ContactManager
 * @param st
 */
ContactManager::ContactManager(SessionState *st)
: state(st) {

}

/**
 * @brief ContactManager::addRequested
 * @param requestId
 * @param status
 */
void ContactManager::addRequested(long requestId, const QString &status) {

    Contact contact;
    contact.contactId = contactsDb->getNextContactId();
    contact.requestId = requestId;
    contact.status = status;
    contact.currentKey = contact.currentSequence = 0;
    contactsDb->addContact(contact, state);
    requestMap[contact.requestId] = contact.contactId;
    contactMap[contact.contactId] = contact;

}

/**
 * @brief ContactManager::loadContactList
 * @param list
 */
void ContactManager::loadContactList(ContactList &list) {

    for (ContactConstIter it = contactMap.begin(); it != contactMap.end(); ++it) {
        list.push_back(it->second);
    }

}

/**
 * @brief ContactManager::loadContacts
 * @param db
 */
void ContactManager::loadContacts(ContactsDatabase *db) {

    contactsDb = db;
    ContactList list;
    try {
        contactsDb->getContacts(list, state);
        for (const auto& contact : list) {
            requestMap[contact.requestId] = contact.contactId;
            contactMap[contact.contactId] = contact;
        }
    }
    catch (DatabaseException& e) {
        QString prefix("Database error while loading contacts - ");
        StatusController::instance()->updateStatus(StatusController::error, prefix + StringCodec(e.what()));
    }

}

/**
 * @brief ContactManager::processContact
 * @param request
 */
void ContactManager::processContact(ContactRequestIn &request) {

    ReqConstIter rit = requestMap.find(request.requestId);
    if (rit != requestMap.end()) {
        qDebug() << "Request " << request.requestId << " already processed";
    }
    else {
        ContactConstIter cit = contactMap.find(rit->second);
        if (cit != contactMap.end()) {
            qDebug() << "Contact ID/Request ID mismatch";
        }
        else {
            Contact contact;
            contact.contactId = contactsDb->getNextContactId();
            contact.requestId = request.requestId;
            contact.contactOf = request.requested.nickname;
            contact.entity.nickname = request.requesting.nickname;
            contact.entity.publicId = request.requesting.publicId;
            contact.currentKey = contact.currentSequence = 0;
            contactsDb->addContact(contact, state);
            requestMap[contact.requestId] = contact.contactId;
            contactMap[contact.contactId] = contact;
        }
    }

}

/**
 * @brief ContactManager::updateContact
 * @param request
 */
void ContactManager::updateContact(ContactRequestIn &request) {

    ReqConstIter rit = requestMap.find(request.requestId);
    if (rit != requestMap.end()) {
        ContactIter cit = contactMap.find(rit->second);
        if (cit != contactMap.end()) {
            Contact& contact = cit->second;
            contact.rsaKeys = request.rsaKeys;

            CK::GCMCodec codec(ByteCodec(request.keyBlock));
            codec.decrypt(state->enclaveKey, state->authData);
            codec >> contact.nonce >> contact.authData;
            unsigned keyCount;
            codec >> keyCount;
            coder::ByteArray key;
            while (contact.messageKeys.size() < keyCount) {
                codec >> key;
                contact.messageKeys.push_back(key);
            }
            contactsDb->updateContact(contact, state);
        }
        else {
            qDebug() << "Contact ID " << rit->second << " not found";
        }
    }
    else {
        qDebug() << "Request ID " << request.requestId << " not found";
    }

}

}
