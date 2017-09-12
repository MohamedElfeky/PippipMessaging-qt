/*
 * ContactDirector.cpp
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

#include "ContactDirector.h"
#include "ContactManager.h"
#include "ContactsDatabase.h"
#include "DatabaseImpl.h"
#include "DatabaseException.h"
#include "GetRequestsTask.h"
#include "Constants.h"
#include "StatusController.h"
#include "ContactRequest.h"
#include "EnclaveException.h"
#include "StringCodec.h"
#include <QTimer>

namespace Pippip {

static const int PROCESS_INTERVAL = 15;

ContactDirector::ContactDirector(SessionState *st, QObject *parent)
: QObject(parent),
  state(st) {

    contactManager = new ContactManager(state);
    getRequestsTask = new GetRequestsTask(state, this);
    connect(getRequestsTask, SIGNAL(getRequestsComplete()), this, SLOT(getRequestsComplete()));
    connect(getRequestsTask, SIGNAL(getRequestsFailed(QString)), this, SLOT(getRequestsFailed(QString)));

}

ContactDirector::~ContactDirector() {

    delete contactManager;
    delete contactsDb;

}

void ContactDirector::createDatabase(const QString &accountName) {

    contactsDb = DatabaseImpl::createContactsDatabase(accountName);
    contactsDb->open(accountName);
    contactManager->loadContacts(contactsDb);   // Do this to set the contacts database reference

}

void ContactDirector::end() {

    contactsDb->close();

}

void ContactDirector::getRequests() {

    try {
        getRequestsTask->doRequest();
    }
    catch (EnclaveException& e) {
        StatusController::instance()->updateStatus(Constants::REDX_ICON, StringCodec(e.what()));
        QTimer::singleShot(PROCESS_INTERVAL *1000, this, SLOT(getRequests()));
    }

}

void ContactDirector::getRequestsComplete() {

    try {
        RequestList requests = getRequestsTask->getRequestList();
        for (auto request : requests) {
            contactManager->processContact(request);
        }
    }
    catch (DatabaseException& e) {
        QString prefix("Database error while processing contacts - ");
        StatusController::instance()->updateStatus(Constants::REDX_ICON, prefix + StringCodec(e.what()));
    }

    QTimer::singleShot(PROCESS_INTERVAL *1000, this, SLOT(getRequests()));

}

void ContactDirector::getRequestsFailed(const QString &error) {

    StatusController::instance()->updateStatus(Constants::REDX_ICON, error);
    QTimer::singleShot(PROCESS_INTERVAL *1000, this, SLOT(getRequests()));

}

void ContactDirector::openDatabase(const QString &accountName) {

    contactsDb = DatabaseImpl::openContactsDatabase(accountName);
    contactManager->loadContacts(contactsDb);

}

void ContactDirector::start() {

    // Starts the get requests cycle.
    getRequests();

}

}
