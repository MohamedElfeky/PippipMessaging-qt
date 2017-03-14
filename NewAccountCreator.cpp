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
#include "RESTHandler.h"
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <QMessageBox>
#include <QApplication>
#include <QJsonObject>
#include <QJsonValue>
#include <sstream>

namespace Pippip {

static const QString REQUEST_URL = "https://pippip.io:2880/newaccount";

NewAccountCreator::NewAccountCreator(NewAccountDialog *parent, SessionState *sess)
: QObject(parent),
  session(sess),
  dialog(parent),
  generator(0) {

    connect(session, SIGNAL(sessionStarted()), this, SLOT(sessionStarted()));
    connect(session, SIGNAL(sessionFailed(QString)), this, SLOT(sessionFailed(QString)));
    connect(this, SIGNAL(incrementProgress(int)), dialog, SLOT(incrementProgress(int)));
    connect(this, SIGNAL(updateInfo(QString)), dialog, SLOT(updateInfo(QString)));

}

NewAccountCreator::~NewAccountCreator() {

    delete generator;

}

void NewAccountCreator::createNewAccount(const QString &name, const QString &pass) {

    accountName = name.toUtf8().constData();
    passphrase = pass.toUtf8().constData();

    emit updateInfo("Generating account paramters");

    generator = new ParameterGenerator(this);
    generator->generateParameters(accountName);
    session->setAccountParameters(*generator);

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

void NewAccountCreator::doAccountRequest() {

    QJsonObject json;

    json["sessionId"] = session->getSessionId();
    json["publicId"] = generator->getPublicId().toHexString().c_str();
    CK::PEMCodec codec;
    std::ostringstream pemstr;
    codec.encode(pemstr, *generator->getUserPublicKey());
    json["userPublicKey"] = pemstr.str().c_str();

    RESTHandler *handler = new RESTHandler;
    RESTTimer *timer = new RESTTimer(this);
    timer->addSource(handler, SIGNAL(requestComplete(RESTHandler*)));
    timer->addSource(handler, SIGNAL(requestFailed(RESTHandler*)));
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(sessionFailed(QString)));

    handler->doPost(REQUEST_URL, json);
    if (!timer->wait(30000)) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Request Error");
        message->setText("New account request timed out");
        message->setIcon(QMessageBox::Critical);
        message->exec();
        emit updateInfo("Unable to complete the request");
    }

}

void NewAccountCreator::progress() {

    emit incrementProgress(5);
    qApp->processEvents();

}

void NewAccountCreator::requestComplete(RESTHandler *handler) {

    progress();
    QJsonObject json = handler->getResponse();

    QJsonValue value = json["error"];
    if (value != QJsonValue::Undefined) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Request Error");
        message->setText("An error occurred while processing the request");
        message->setInformativeText(value.toString());
        message->setIcon(QMessageBox::Critical);
        message->exec();
    }
    else {
        QJsonValue keyValue = json["serverPublicKey"];
        QJsonValue encValue = json["encrypted"];
        if (keyValue == QJsonValue::Undefined || encValue == QJsonValue::Undefined) {
            QMessageBox *message = new QMessageBox;
            message->addButton(QMessageBox::Ok);
            message->setWindowTitle("Request Error");
            message->setText("An error occurred while processing the request");
            message->setInformativeText("Invalid server response");
            message->setIcon(QMessageBox::Critical);
            message->exec();
        }
        else {
            CK::PEMCodec codec;
            // CK::RSAPublicKey *key = codec.decodePublicKey(keyValue.toString());

        }
    }

}

void NewAccountCreator::requestFailed(RESTHandler *handler) {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("Request Error");
    message->setText("An error occurred while processing the request");
    message->setInformativeText(handler->getError());
    message->setIcon(QMessageBox::Critical);
    message->exec();
    emit updateInfo("Unable to complete the Request");

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
    emit incrementProgress(5);
    qApp->processEvents();

    doAccountRequest();

}

bool NewAccountCreator::startSession() {

    RESTTimer *sessionTimer = new RESTTimer(this);
    sessionTimer->addSource(session, SIGNAL(sessionStarted()));
    sessionTimer->addSource(session, SIGNAL(sessionFailed(QString)));
    session->startSession();
    return sessionTimer->wait(30000);

}

}
