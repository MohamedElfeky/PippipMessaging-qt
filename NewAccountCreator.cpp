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
#include "NewAccountDialog.h"
#include "SessionState.h"
#include "ParameterGenerator.h"
#include "RESTTimer.h"
#include <QMessageBox>

namespace Pippip {

NewAccountCreator::NewAccountCreator(NewAccountDialog *parent, SessionState *sess)
: QObject(parent),
  session(sess),
  dialog(parent) {

    sessionTimer = new RESTTimer(this);
    sessionTimer->addSource(session, SIGNAL(sessionStarted()));
    sessionTimer->addSource(session, SIGNAL(sessionFailed(QString)));
    connect(session, SIGNAL(sessionStarted()), this, SLOT(sessionStarted()));
    connect(session, SIGNAL(sessionFailed(QString)), this, SLOT(sessionFailed(QString)));
    connect(this, SIGNAL(incrementProgress(int)), dialog, SLOT(incrementProgress(int)));
    connect(this, SIGNAL(updateInfo(QString)), dialog, SLOT(updateInfo(QString)));

}

NewAccountCreator::~NewAccountCreator() {
}

void NewAccountCreator::createNewAccount(const QString &name, const QString &pass) {

    accountName = name.toUtf8().constData();
    passphrase = pass.toUtf8().constData();

    emit updateInfo("Generating account paramters");

    ParameterGenerator gen;
    gen.generateParameters(accountName);
    session->setAccountParameters(gen);

    emit updateInfo("Contacting the server");
    if (!startSession()) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Session Error");
        message->setText("Session request timed out");
        message->setIcon(QMessageBox::Critical);
        message->exec();
        emit updateInfo("Unable to establish a session with the server");
    }

}

void NewAccountCreator::sessionFailed(QString error) {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("Session Error");
    message->setText("An error occurred while establishing a session with the server.");
    message->setInformativeText(error);
    message->setIcon(QMessageBox::Critical);
    message->exec();
    emit updateInfo("Unable to establish a session with the server");

}

void NewAccountCreator::sessionStarted() {

    emit updateInfo("Session started");

}

bool NewAccountCreator::startSession() {

    session->startSession();
    return sessionTimer->wait(30000);

}

}
