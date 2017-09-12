#include "SQLLiteContactsDatabaseImpl.h"
#include "SessionState.h"
#include "StringCodec.h"
#include "ByteCodec.h"
#include "Contact.h"
#include "Function.h"
#include "DatabaseException.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <CryptoKitty-C/encoding/GCMCodec.h>
#include <CryptoKitty-C/exceptions/EncodingException.h>
#include <sstream>

namespace Pippip {

struct DatabaseContact {
    qint32 contactId;
    qint64 requestId;
    QByteArray contact;
};

bool SQLLiteContactsDatabaseImpl::once = true;

/**
 * @brief SQLLiteContactsDatabaseImpl::SQLLiteContactsDatabaseImpl
 */
SQLLiteContactsDatabaseImpl::SQLLiteContactsDatabaseImpl()
: closed(true) {

}

/**
 * @brief SQLLiteContactsDatabaseImpl::addContact
 * @param contact
 * @param state
 */
void SQLLiteContactsDatabaseImpl::addContact(const Contact& contact, SessionState *state) {

    if (closed) {
        throw DatabaseException("Database closed");
    }

    if (contactExists(contact.contactId)) {
        throw DatabaseException("Contact exists");
    }

    DatabaseContact dbContact;
    dbContact.contactId = contact.contactId;
    dbContact.requestId = contact.requestId;

    try {
        CK::GCMCodec codec;
        codec << contact;
        codec.encrypt(state->contactKey, state->authData);
        dbContact.contact = ByteCodec(codec.toArray());
    }
    catch (CK::EncodingException& e) {
        std::ostringstream estr;
        estr << "Encoding/Encryption error in add contact - " << e.what();
        throw DatabaseException(estr.str());
    }

    QSqlQuery query(database);
    query.prepare("INSERT INTO contacts (contactId, requestId, contact) VALUES (:contactId, :requestId, :contact)");
    query.bindValue(":contactId", dbContact.contactId);
    query.bindValue(":requestId", dbContact.requestId);
    query.bindValue(":contact", dbContact.contact);
    query.exec();
    if (!query.isActive()) {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

}

/**
 * @brief SQLLiteContactsDatabaseImpl::close
 */
void SQLLiteContactsDatabaseImpl::close() {

    database.close();
    closed = true;

}

/**
 * @brief SQLLiteContactsDatabaseImpl::contactExists
 * @param contactId
 * @return
 */
bool SQLLiteContactsDatabaseImpl::contactExists(int contactId) {

    QSqlQuery query(database);
    query.prepare("SELECT requestId FROM contacts WHERE contactId = :contactId");
    query.bindValue(":contactId", contactId);
    query.exec();
    if (!query.isActive()) {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

    return query.next();

}

/**
 * @brief SQLLiteContactsDatabaseImpl::create
 * @param account
 */
void SQLLiteContactsDatabaseImpl::create(const QString &account) {

    initialize();
    QSqlDatabase db = QSqlDatabase::database("contacts", false);
    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    db.setDatabaseName(dbPath + "/" + account + ".cdb");
    if (db.open()) {
        QSqlQuery contactsQuery("CREATE TABLE contacts (contactId INTEGER PRIMARY KEY, "
                        "requestId INTEGER, contact BLOB)", db);
        if (!contactsQuery.isActive()) {
            QString error = contactsQuery.lastError().text();
            db.close();
            throw DatabaseException(StringCodec(error));
        }
        QSqlQuery idQuery("CREATE TABLE ids (contactId INTEGER)", db);
        if (!idQuery.isActive()) {
            QString error = idQuery.lastError().text();
            db.close();
            throw DatabaseException(StringCodec(error));
        }
        QSqlQuery initQuery("INSERT INTO ids (contactId) VALUES(1)", db);
        if (!initQuery.isActive()) {
            QString error = initQuery.lastError().text();
            db.close();
            throw DatabaseException(StringCodec(error));
        }
        db.close();
    }
    else {
        throw DatabaseException(StringCodec(db.lastError().text()));
    }

}

/**
 * @brief SQLLiteContactsDatabaseImpl::getContacts
 * @param list
 * @param state
 */
void SQLLiteContactsDatabaseImpl::getContacts(ContactList& list, SessionState *state) {

    if (closed) {
        throw DatabaseException("Database closed");
    }

    QSqlQuery query(database);
    query.setForwardOnly(true);
    query.exec("SELECT * FROM contacts");
    if (query.isActive()) {
        while (query.next()) {
            DatabaseContact dbContact;
            dbContact.contactId = query.value(0).toInt();
            dbContact.requestId = query.value(1).toLongLong();
            dbContact.contact = query.value(2).toByteArray();

            try {
                Contact contact;
                CK::GCMCodec codec(ByteCodec(dbContact.contact));
                codec.decrypt(state->contactKey, state->authData);
                codec >> contact;
                list.push_back(contact);
            }
            catch (CK::EncodingException& e) {
                std::ostringstream estr;
                estr << "Encoding/Encryption error in get contacts - " << e.what();
                throw DatabaseException(estr.str());
            }
        }
    }
    else {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

}

/**
 * @brief SQLLiteContactsDatabaseImpl::getNextContactId
 * @return
 */
int SQLLiteContactsDatabaseImpl::getNextContactId() {

    if (closed) {
        throw DatabaseException("Database closed");
    }

    QSqlQuery getQuery(database);
    getQuery.setForwardOnly(true);
    getQuery.exec("SELECT contactId FROM ids");
    int contactId;
    if (getQuery.isActive()) {
        getQuery.next();
        contactId = getQuery.value(0).toInt();
    }
    else {
        throw DatabaseException(StringCodec(getQuery.lastError().text()));
    }

    QSqlQuery setQuery(database);
    setQuery.prepare("UPDATE ids SET contactId = :contactId");
    setQuery.bindValue(":contactId", contactId + 1);
    setQuery.exec();
    if (!setQuery.isActive()) {
        throw DatabaseException(StringCodec(setQuery.lastError().text()));
    }

    return contactId;

}

/**
 * @brief SQLLiteContactsDatabaseImpl::initialize
 */
void SQLLiteContactsDatabaseImpl::initialize() {

    if (once) {
        once = false;
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "contacts");
        if (!db.isValid()) {
            throw DatabaseException("Database driver not available");
        }
    }

}

/**
 * @brief SQLLiteContactsDatabaseImpl::open
 * @param account
 */
void SQLLiteContactsDatabaseImpl::open(const QString& account) {

    initialize();
    database = QSqlDatabase::database("contacts");
    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    database.setDatabaseName(dbPath + "/" + account + ".cdb");
    if (!database.open()) {
        throw DatabaseException(StringCodec(database.lastError().text()));
    }
    closed = false;

}

/**
 * @brief SQLLiteContactsDatabaseImpl::updateContact
 * @param contact
 * @param state
 */
void SQLLiteContactsDatabaseImpl:: updateContact(const Contact &contact, SessionState *state) {

    if (closed) {
        throw DatabaseException("Database closed");
    }

    CK::GCMCodec codec;
    try {
        codec << contact;
        codec.encrypt(state->contactKey, state->authData);
    }
    catch (CK::EncodingException& e) {
        std::ostringstream estr;
        estr << "Encoding/Encryption error in update contacts - " << e.what();
        throw DatabaseException(estr.str());
    }

    QSqlQuery query(database);
    query.prepare("UPDATE contacts SET contact = :encoded WHERE contactId = :contactId");
    query.bindValue(":contactId", contact.contactId);
    query.bindValue(":encoded", ByteCodec(codec.toArray()).getQtBytes());
    query.exec();
    if (!query.isActive()) {
        throw DatabaseException(StringCodec(query.lastError().text()));
    }

}

}
