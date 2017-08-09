#include "ContactsDatabase.h"
#include "SessionState.h"
#include "DatabaseException.h"
#include "StringCodec.h"
#include "ByteCodec.h"
#include "Contacts.h"
#include "Function.h"
#include "SessionState.h"
#include "ContactManager.h"
#include "Constants.h"
#include "ContactRequests.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <QFile>
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>
#include <sstream>
#include <cstdlib>

namespace Pippip {

static const int ID_MASK = 0x7fffffff;

bool ContactsDatabase::once = true;

ContactsDatabase::ContactsDatabase(SessionState *st, QObject *parent)
: QObject(parent),
  state(st),
  contacts(new Contacts) {

    contactManager = new ContactManager(state, this);
    connect(contactManager, SIGNAL(contactsLoaded()), this, SLOT(contactsLoaded()));
    connect(contactManager, SIGNAL(loadFailed(QString)), this, SLOT(loadFailed(QString)));
    connect(contactManager, SIGNAL(contactRequestComplete(long,QString)),
            this, SLOT(contactRequestComplete(long,QString)));

}

void ContactsDatabase::addPending() {

    CK::FortunaSecureRandom rnd;
    try {
        pending.contactId = rnd.nextUnsignedInt() & ID_MASK;
        while (contactExists(pending.contactId)) {
            pending.contactId = rnd.nextUnsignedInt() & ID_MASK;
        }

        CK::GCMCodec codec;
        codec << pending;
        codec.encrypt(state->contactKey, state->authData);
        QSqlQuery query(database);
        query.prepare("INSERT INTO contacts (id, contact) VALUES (:id, :contact)");
        query.bindValue(":id", pending.contactId);
        ByteCodec encoded(codec.toArray());
        query.bindValue(":contact", encoded.getQtBytes());
        query.exec();
        if (!query.isActive()) {
            emit addContactFailed(QString("Database error - ") + query.lastError().text());
        }
        contacts->add(pending);
        contactManager->addContact(pending);
    }
    catch (CK::EncodingException& e) {
        emit addContactFailed(QString("Encoding error - ") + e.what());
    }
    catch (DatabaseException& e) {
        emit addContactFailed(QString("Database error - ") + e.what());
    }

}

bool ContactsDatabase::contactExists(unsigned id) {

    QSqlQuery query(database);
    query.prepare("SELECT contact FROM contacts WHERE id = :id");
    query.bindValue(":id", id);
    query.exec();
    if (query.isActive()) {
        return query.next();
    }
    else {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

}

void ContactsDatabase::contactRequestComplete(long requestId, const QString &status) {

    pending.requestId = requestId;
    pending.status = status;
    addPending();


}

/*
 * Reconcile local contacts with server contacts.
 *
 * If the local database is empty, it will be filled with the contents of
 * the server database. Otherwise, the local database is the source of
 * record.
 */
void ContactsDatabase::contactsLoaded() {

    if (contacts->size() == 0) {
        Contacts *cmContacts = contactManager->getContacts();
        contacts->fill(*cmContacts);
    }
    else {
        contactManager->reconcile(*contacts);
    }

}

void ContactsDatabase::close() {

    database.close();
    delete contacts;

}

/*
 * This is a new contact created in the acknowledge requests task.
 * The user is the requested entity.
 */
void ContactsDatabase::createContact(Contact& contact, const ContactRequestIn &request) {

    contact.contactOf = request.requested.nickname;
    contact.entity = request.requesting;
    contact.rsaKeys = request.rsaKeys;

    CK::GCMCodec codec(request.keyBlock);
    codec.decrypt(state->enclaveKey, state->authData);
    contact.nonce.setLength(8);
    codec.getBytes(contact.nonce);
    contact.authData.setLength(16);
    codec.getBytes(contact.authData);
    coder::ByteArray key(32, 0);
    while (contact.messageKeys.size() < 10) {
        codec.getBytes(key);
        contact.messageKeys.push_back(key);
    }
    contact.currentKey = contact.currentSequence = 0;

}

void ContactsDatabase::createDatabase(const QString &account) {

    QSqlDatabase db = QSqlDatabase::database("contacts", false);
    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    db.setDatabaseName(dbPath + "/" + account + ".cdb");
    if (db.open()) {
        QSqlQuery query("CREATE TABLE contacts (id INTEGER PRIMARY KEY, "
                        "contact BLOB)", db);
        if (!query.isActive()) {
            StringCodec error(query.lastError().text());
            db.close();
            throw DatabaseException(error);
        }
        db.close();
    }
    else {
        StringCodec error(db.lastError().text());
        throw DatabaseException(error);
    }

}

void ContactsDatabase::initialize(const QString& account) {

    if (once) {
        once = false;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "contacts");
        if (!db.isValid()) {
            throw DatabaseException("Database driver not available");
        }
    }

    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    QFile dbFile(dbPath + "/" + account + ".cdb");
    if (!dbFile.exists()) {
        createDatabase(account);
    }

}

void ContactsDatabase::loadContacts() {

    QSqlQuery query(database);
    query.setForwardOnly(true);
    query.exec("SELECT * FROM contacts");
    if (query.isActive()) {
        while (query.next()) {
            Contact contact;
            contact.contactId = query.value(0).toInt();
            QByteArray encoded = query.value(1).toByteArray();
            ByteCodec bytes(encoded);
            try {
                CK::GCMCodec codec(bytes);
                codec.decrypt(state->contactKey, state->authData);
                codec >> contact;
                contacts->add(contact);
            }
            catch (CK::EncodingException& e) {
                std::ostringstream estr;
                estr << "Error decoding contact - " << e.what();
                throw DatabaseException(estr.str());
            }
        }
    }
    else {
        StringCodec error(query.lastError().text());
        throw DatabaseException(error);
    }

    contactManager->loadContacts();

}

void ContactsDatabase::loadFailed(const QString &error) {

    emit updateStatus(Constants::REDX_ICON, "Contacts load failed - " + error);

}

void ContactsDatabase::newContact(const ContactRequestIn &request) {

    Contact contact;
    createContact(contact, request);
    CK::GCMCodec codec;
    codec << contact;
    codec.encrypt(state->contactKey, state->authData);

    QSqlQuery query(database);
    query.prepare("INSERT INTO contacts (id, contact) VALUES (:id, :encoded)");
    query.bindValue(":id", contact.contactId);
    query.bindValue(":encoded", ByteCodec(codec.toArray()).getQtBytes());
    query.exec();
    if (!query.isActive()) {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

    contacts->add(contact);

    /*
     * This is a no-fault operation. The contact is either updated or added
     * to the enclave.
     */
    contactManager->addContact(contact);

}

void ContactsDatabase::open(const QString& account) {

    database = QSqlDatabase::database("contacts");
    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    database.setDatabaseName(dbPath + "/" + account + ".cdb");
    if (!database.open()) {
        StringCodec error(database.lastError().text());
        throw DatabaseException(error);
    }

}

void ContactsDatabase::requestContact(const ContactRequestOut &request) {

    if (request.idTypes == Constants::NICKNAME_NICKNAME
            || request.idTypes == Constants::NICKNAME_PUBLICID) {
        pending.contactOf = request.requestingId;
    }
    if (request.idTypes == Constants::NICKNAME_NICKNAME
            || request.idTypes == Constants::PUBLICID_NICKNAME) {
        pending.entity.nickname = request.requestedId;
    }
    else {
        pending.entity.publicId = request.requestedId;
    }
    contactManager->requestContact(request);

}

void ContactsDatabase::requestsAcknowledged(ContactRequests *acknowledged) {

    for (auto request : *acknowledged) {
        if (request.status == "accepted") { // Update or build a contact
            Contact contact;
            if (contacts->fromRequestId(request.requestId, contact)) {
                updateContact(contact, request);
                contacts->replace(contact);
            }
            else {
                newContact(request);
            }
        }
    }

}

/*
 * This will update the provisional contact that was created during the
 * request contact task. The user is the requesting entity.
 */
void ContactsDatabase:: updateContact(Contact& contact, const ContactRequestIn &request) {

    contact.contactOf = request.requesting.nickname;
    contact.entity = request.requested;
    contact.rsaKeys = request.rsaKeys;

    CK::GCMCodec codec(request.keyBlock);
    codec.decrypt(state->enclaveKey, state->authData);
    contact.nonce.setLength(8);
    codec.getBytes(contact.nonce);
    contact.authData.setLength(16);
    codec.getBytes(contact.authData);
    coder::ByteArray key(32, 0);
    while (contact.messageKeys.size() < 10) {
        codec.getBytes(key);
        contact.messageKeys.push_back(key);
    }
    contact.currentKey = contact.currentSequence = 0;

    updateContact(contact);

}

void ContactsDatabase:: updateContact(const Contact &contact) {

    CK::GCMCodec codec;
    codec << contact;
    codec.encrypt(state->contactKey, state->authData);

    QSqlQuery query(database);
    query.prepare("UPDATE contacts SET contact = :encoded WHERE id = :id");
    query.bindValue(":id", contact.contactId);
    query.bindValue(":encoded", ByteCodec(codec.toArray()).getQtBytes());
    query.exec();
    if (!query.isActive()) {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

    /*
     * This is a no-fault operation. The contact is either updated or added
     * to the enclave.
     */
    contactManager->addContact(contact);

}

}
