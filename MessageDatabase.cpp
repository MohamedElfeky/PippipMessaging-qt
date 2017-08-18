/*
 * MessageDatabase.cpp
 * Copyright (C) 2017 Steve Brenneis <steve.brenneis@secomm.org>
 *
 * PippipMessaging is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * PippipMessaging is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MessageDatabase.h"
#include "SessionState.h"
#include "StringCodec.h"
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <iostream>

namespace Pippip {

MessageDatabase::MessageDatabase() {

//    database = QSqlDatabase::addDatabase("QSQLITE");

}

MessageDatabase::~MessageDatabase() {
}

bool MessageDatabase::addMessage(const Message &message) {

    QSqlQuery query;
    query.prepare("INSERT INTO messages(id, version, timestamp, sender, recipient, recipientId, keyIndex, message) "
                  "VALUES(?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(message.messageId);
    query.addBindValue(message.version);
    query.addBindValue(message.timestamp);
    query.addBindValue(message.sender);
    query.addBindValue(message.recipient);
    query.addBindValue(message.recipientId);
    query.addBindValue(message.keyIndex);
    query.addBindValue(message.message);
    if (!query.exec()) {
        lastError = query.lastError().text();
        return false;
    }
    return true;

}

void MessageDatabase::close() {

    database.close();

}

bool MessageDatabase::create(SessionState *state) {

    bool success = false;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    db.setDatabaseName(dbPath + "/" + state->accountName + ".mdb");
    if (db.open()) {
        QSqlQuery query("CREATE TABLE messages (id INTEGER PRIMARY KEY, version TEXT, "
                        "timestamp INTEGER, sender TEXT, recipient TEXT, recipientId TEXT, "
                        "keyIndex INTEGER, message BLOB)");
        success = query.isActive();
        if (!success) {
            std::cout << "Failed to create message database for "
                      << StringCodec(state->accountName).getStdString() << ", reason: "
                      << StringCodec(query.lastError().text()).getStdString() << std::endl;
        }
        db.close();
    }
    return success;

}

void MessageDatabase::initialize(/* SessionState *state */) {

    QSqlDatabase::addDatabase("QSQLITE", "messages");

}

bool MessageDatabase::open(const QString &account) {

    QSettings settings;
    QString dbPath = settings.value("Defaults/dbPath").toString();
    database.setDatabaseName(dbPath + account + ".mdb");

    return database.open();

}

}

