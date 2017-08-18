#include "ContactsDatabase.h"
#include "SessionState.h"
#include "StringCodec.h"
#include "ByteCodec.h"
#include "Contacts.h"
#include "Function.h"
#include "SessionState.h"
#include "Constants.h"
#include "ContactRequests.h"
#include "GetContactsTask.h"
#include "AddContactsTask.h"
#include "DatabaseException.h"
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

ContactsDatabase::ContactsDatabase(SessionState *st)
: state(st) {

/*    contactManager = new ContactManager(state, this);
    connect(contactManager, SIGNAL(contactsLoaded()), this, SLOT(contactsLoaded()));
    connect(contactManager, SIGNAL(loadFailed(QString)), this, SLOT(loadFailed(QString)));
    connect(contactManager, SIGNAL(contactRequestComplete(long,QString)),
            this, SLOT(contactRequestComplete(long,QString)));
*/
}

void ContactsDatabase::addContact(DatabaseContact& contact) {

    CK::FortunaSecureRandom rnd;
    if (contact.id == 0) {
        contact.id = rnd.nextUnsignedInt() & ID_MASK;
        while (contactExists(contact.id)) {
            contact.id = rnd.nextUnsignedInt() & ID_MASK;
        }
    }

    QSqlQuery query(database);
    query.prepare("INSERT INTO contacts (id, contact) VALUES (:id, :contact)");
    query.bindValue(":id", contact.id);
    query.bindValue(":contact", contact.contact);
    query.exec();
    if (!query.isActive()) {
        throw DatabaseException(StringCodec(query.lastError().text()));
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

/*
 * Reconcile local contacts with server contacts.
 *
 * If the local database is empty, it will be filled with the contents of
 * the server database. Otherwise, the local database is the source of
 * record.
 */
void ContactsDatabase::close() {

    database.close();

}

/*
 * This is a new contact created in the acknowledge requests task.
 * The user is the requested entity.

void ContactsDatabase::createContact(Contact& contact, const ContactRequestIn &request) {

    contact.status = request.status;
    contact.contactOf = request.requested.nickname;
    contact.entity = request.requesting;
    contact.rsaKeys = request.rsaKeys;

    std::cout << "Key block in = " << request.keyBlock.toHexString() << std::endl;
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
*/
void ContactsDatabase::createDatabase(const QString &account) {

    QSqlDatabase db = QSqlDatabase::database("contacts", false);
    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    db.setDatabaseName(dbPath + "/" + account + ".cdb");
    if (db.open()) {
        QSqlQuery query("CREATE TABLE contacts (id INTEGER PRIMARY KEY, "
                        "contact BLOB)", db);
        if (!query.isActive()) {
            QString error = query.lastError().text();
            db.close();
            throw DatabaseException(StringCodec(error));
        }
        db.close();
    }
    else {
        throw DatabaseException(StringCodec(db.lastError().text()));
    }

}

void ContactsDatabase::getContacts(DatabaseContactList& list) {

    QSqlQuery query(database);
    query.setForwardOnly(true);
    query.exec("SELECT * FROM contacts");
    if (query.isActive()) {
        while (query.next()) {
            DatabaseContact contact;
            contact.id = query.value(0).toInt();
            contact.contact = query.value(1).toByteArray();
        }
    }
    else {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

}

void ContactsDatabase::initialize(SessionState *state) {

    if (once) {
        once = false;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "contacts");
        if (!db.isValid()) {
            throw DatabaseException("Database driver not available");
        }
    }

    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    QFile dbFile(dbPath + "/" + state->accountName + ".cdb");
    if (!dbFile.exists()) {
        createDatabase(state->accountName);
    }

}
/*
void ContactsDatabase::insertContacts(const ServerContactList &contacts) {

    for (auto serverContact : contacts) {
        QSqlQuery query(database);
        query.prepare("INSERT INTO contacts (id, contact) VALUES (:id, :contact)");
        query.bindValue(":id", serverContact.contactId);
        query.bindValue(":contact", serverContact.contact);
        query.exec();
        if (!query.isActive()) {
            emit databaseError(query.lastError().text());
        }
        else {
            try {
                ByteCodec bytes(serverContact.contact);
                CK::GCMCodec codec(bytes);
                codec.decrypt(state->contactKey, state->authData);
                Contact contact;
                codec >> contact;
                state->contacts->add(contact);
            }
            catch (CK::EncodingException& e) {
                emit databaseError(QString("Error decoding contact - ") + e.what());
            }
        }
    }

}

void ContactsDatabase::loadServerContacts() {

    LoadContactsTask *task = new LoadContactsTask(state, this);
    connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestCompleted(Pippip::EnclaveRequestTask*)));
    connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
    task->doRequest();

}

void ContactsDatabase::newContact(const ContactRequestIn &request) {

    try {
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
            emit databaseError(query.lastError().text());
        }

        //contacts->add(contact);

        /*
     * This is a no-fault operation. The contact is either updated or added
     * to the enclave.

        //contactManager->updateContact(contact);
    }
    catch (CK::EncodingException& e) {
        emit updateStatus(Constants::REDX_ICON, QString("Failed to create new contact - ")
                          + e.what());
    }

}
*/
ContactsDatabase *ContactsDatabase::open(SessionState *state) {

    ContactsDatabase *database = new ContactsDatabase(state);
    database->database = QSqlDatabase::database("contacts");
    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    database->database.setDatabaseName(dbPath + "/" + state->accountName + ".cdb");
    if (!database->database.open()) {
        throw DatabaseException(StringCodec(database->database.lastError().text()));
    }
    else {
        return database;
    }

}
/*
void ContactsDatabase::requestCompleted(EnclaveRequestTask *task) {

    QString status;
    QString taskName = task->getTaskName();
    if (taskName == Constants::LOAD_CONTACTS_TASK) {
        status = "Contacts loaded";
        LoadContactsTask *loadTask = dynamic_cast<LoadContactsTask*>(task);
        resolveContacts(loadTask->getContacts());
    }
    else if (taskName == Constants::ADD_CONTACTS_TASK) {
        status = "Contacts added";
    }

    emit updateStatus(Constants::CHECK_ICON, status);

}

void ContactsDatabase::requestFailed(EnclaveRequestTask *task) {

    QString status;
    QString taskName = task->getTaskName();
    if (taskName == Constants::LOAD_CONTACTS_TASK) {
        status = "Failed to load contacts - " + task->getError();
    }
    else if (taskName == Constants::ADD_CONTACTS_TASK) {
        status = "Failed to add contacts - " + task->getError();
    }

    emit updateStatus(Constants::REDX_ICON, status);

}

void ContactsDatabase::requestsAcknowledged(ContactRequests *acknowledged) {

    for (auto request : *acknowledged) {
        if (request.status == "active") { // Update or build a contact
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

void ContactsDatabase::resolveContacts(const ServerContactList& contacts) {

    //try {
        if (state->contacts->size() == 0) {
            // Restoring contacts from the enclave.
            insertContacts(contacts);
            emit contactsModified();
        }
        // TODO Reconcile contacts?
    //}
    //catch (DatabaseException& e) {
    //    emit updateStatus(Constants::REDBAR_ICON, QString(e.what()));
    //}

}

void ContactsDatabase::updateContact(const ContactRequestIn &request) {
/*
    Contact contact;
    if (contacts->fromRequestId(request.requestId, contact)) {
        updateContact(contact, request);
    }
    else {
        emit updateStatus(Constants::CAUTION_ICON, "Contact not found");
    }

}

/*
 * This will update the provisional contact that was created during the
 * request contact task. The user is the requesting entity.

void ContactsDatabase::updateContact(Contact& contact, const ContactRequestIn &request) {

    contact.status = request.status;
    contact.contactOf = request.requesting.nickname;
    contact.entity = request.requested;
    contact.rsaKeys = request.rsaKeys;

    try {
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
    catch (CK::EncodingException& e) {
        emit updateStatus(Constants::REDX_ICON, QString("Failed to update contact - ")
                          + e.what());
    }

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
        //throw DatabaseException(StringCodec(query.lastError().text()));
    }

    /*
     * This is a no-fault operation. The contact is either updated or added
     * to the enclave.

    //contactManager->updateContact(contact);

}
*/
}
