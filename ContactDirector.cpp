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

#include "AckRequestsTask.h"
#include "Constants.h"
#include "ContactDirector.h"
#include "ContactManager.h"
#include "ContactRequest.h"
#include "ContactsDatabase.h"
#include "DatabaseImpl.h"
#include "DatabaseException.h"
#include "EnclaveException.h"
#include "GetRequestsTask.h"
#include "StatusController.h"
#include "StringCodec.h"
#include <QTimer>

namespace Pippip {

static const int PROCESS_INTERVAL = 15;

/**
 * @brief ContactDirector::ContactDirector
 * @param st
 * @param parent
 */
ContactDirector::ContactDirector(SessionState *st, QObject *parent)
: QObject(parent),
  state(st) {

    contactManager = new ContactManager(state);
    getRequestsTask = new GetRequestsTask(state, this);
    connect(getRequestsTask, SIGNAL(getRequestsComplete(QString)), this, SLOT(getRequestsComplete(QString)));
    connect(getRequestsTask, SIGNAL(getRequestsFailed(QString)), this, SLOT(getRequestsFailed(QString)));
    connect(getRequestsTask, SIGNAL(enclaveRequestTimedOut()), this, SLOT(requestTimedOut()));
    ackRequestsTask = new AckRequestsTask(state, this);
    connect(ackRequestsTask, SIGNAL(ackRequestsComplete()), this, SLOT(ackRequestsComplete()));
    connect(ackRequestsTask, SIGNAL(ackRequestsFailed(QString)), this, SLOT(ackRequestsFailed(QString)));
    connect(ackRequestsTask, SIGNAL(enclaveRequestTimedOut()), this, SLOT(requestTimedOut()));

}

/**
 * @brief ContactDirector::~ContactDirector
 */
ContactDirector::~ContactDirector() {

    delete contactManager;
    delete contactsDb;

}

/**
 * @brief ContactDirector::createDatabase
 * @param accountName
 */
void ContactDirector::createDatabase(const QString &accountName) {

    contactsDb = DatabaseImpl::createContactsDatabase(accountName);
    contactsDb->open(accountName);
    contactManager->loadContacts(contactsDb);   // Do this to set the contacts database reference

}

/**
 * @brief ContactDirector::end
 */
void ContactDirector::end() {

    contactsDb->close();

}

/**
 * @brief ContactDirector::ackRequestsComplete
 */
void ContactDirector::ackRequestsComplete() {

    const RequestList& requests = ackRequestsTask->getRequestList();
    for (auto request : requests) {
        contactManager->updateContact(request);
    }

}

/**
 * @brief ContactDirector::ackRequestsFailed
 * @param error
 */
void ContactDirector::ackRequestsFailed(const QString &error) {

    StatusController::instance()->updateStatus(StatusController::error, error);

}

/**
 * Gets incoming contact requests. Invoked by the timer expired signal.
 *
 * @brief ContactDirector::getRequests
 */
void ContactDirector::getRequests() {

    try {
        getRequests("requested");
    }
    catch (EnclaveException& e) {
        StatusController::instance()->updateStatus(StatusController::error, StringCodec(e.what()));
        QTimer::singleShot(PROCESS_INTERVAL *1000, this, SLOT(getRequests()));
    }

}

/**
 * Get requests for specified requester. May throw EnclaveException.
 *
 * @brief ContactDirector::getRequests
 * @param requester
 */
void ContactDirector::getRequests(const QString& requester) {

    getRequestsTask->getRequests(requester);

}

/**
 * @brief ContactDirector::getRequestsComplete
 */
void ContactDirector::getRequestsComplete(const QString& requester) {

    try {
        ackRequestsTask->clear();
        RequestList requests = getRequestsTask->getRequestList();
        for (auto request : requests) {
            if (request.status == "pending" && requester == "requested") {
                contactManager->processContact(request);
            }
            else if (request.status == "active" && requester = "requesting") {
                contactManager->updateContact(request);
                ackRequestsTask->addAcknowledgement(request.requestId, "accept");
            }
        }
        ackRequestsTask->acknowledgeRequests(requester);
    }
    catch (DatabaseException& e) {
        QString prefix("Database error while processing contacts - ");
        StatusController::instance()->updateStatus(StatusController::error, prefix + StringCodec(e.what()));
    }

    QTimer::singleShot(PROCESS_INTERVAL *1000, this, SLOT(getRequests()));

}

/**
 * @brief ContactDirector::getRequestsFailed
 * @param error
 */
void ContactDirector::getRequestsFailed(const QString &error) {

    StatusController::instance()->updateStatus(StatusController::error, error);
    QTimer::singleShot(PROCESS_INTERVAL *1000, this, SLOT(getRequests()));

}

/**
 * @brief ContactDirector::openDatabase
 * @param accountName
 */
void ContactDirector::openDatabase(const QString &accountName) {

    contactsDb = DatabaseImpl::openContactsDatabase(accountName);
    contactManager->loadContacts(contactsDb);

}

/**
 * @brief ContactDirector::requestTimedOut
 */
void ContactDirector::requestTimedOut() {

    StatusController::instance()->updateStatus(StatusController::error, "Contact request timed out");

}

/**
 * @brief ContactDirector::start
 */
void ContactDirector::start() {

    // Starts the get requests cycle.
    getRequests();

}

}
