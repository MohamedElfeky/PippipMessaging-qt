/*
 * NewAccountCreator.cpp
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

#include "NewAccountCreator.h"
#include "SessionState.h"
#include "RESTTimer.h"
#include <QMessageBox>

namespace Pippip {

NewAccountCreator::NewAccountCreator(SessionState *sess)
: session(sess) {
}

NewAccountCreator::~NewAccountCreator() {
}

void NewAccountCreator::createNewAccount(const QString &name, const QString &pass) {

    accountName = name.toUtf8().constData();
    passphrase = pass.toUtf8().constData();

    if (startSession()) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Session Success");
        message->setText("Session started.");
        message->setInformativeText(QString(session->getSessionId().c_str()));
        message->setIcon(QMessageBox::Information);
        message->exec();
    }
    else {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Session Error");
        message->setText("An error occurred while establishing a session with the server.");
        message->setInformativeText(session->getError());
        message->setIcon(QMessageBox::Critical);
        message->exec();
    }

}

bool NewAccountCreator::startSession() {

    RESTTimer t1(session, "sessionStarted");   // See sequence diagrams
    session->startSession();
    return t1.wait(30000);

}

}
