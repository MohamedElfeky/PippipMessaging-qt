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
#include "ContactRequest.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <QFile>
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>
#include <sstream>

namespace Pippip {

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
        pending.contactId = rnd.nextUnsignedInt();
        while (contactExists(pending.contactId)) {
            pending.contactId = rnd.nextUnsignedInt();
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

    emit updateStatus(Constants::REDX_ICON + "<big>Contacts load failed - " + error);

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

void ContactsDatabase::requestContact(const ContactRequest &request) {

    if (request.idTypes == NICKNAME_NICKNAME || request.idTypes == NICKNAME_PUBLICID) {
        pending.contactOf = request.requestingId;
    }
    if (request.idTypes == NICKNAME_NICKNAME || request.idTypes == PUBLICID_NICKNAME) {
        pending.entity.nickname = request.requestedId;
    }
    else {
        pending.entity.publicId = request.requestedId;
    }
    contactManager->requestContact(request);

}

}
