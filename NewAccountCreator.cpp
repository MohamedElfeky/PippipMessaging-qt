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
#include "StringCodec.h"
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <CryptoKitty-C/encoding/RSACodec.h>
#include <coder/ByteArray.h>
#include <QMessageBox>
#include <QApplication>
#include <QJsonObject>
#include <QJsonValue>
#include <sstream>

namespace Pippip {

static const QString REQUEST_URL = "https://pippip.io:2880/newaccount/";
static const QString FINISH_URL = "https://pippip.io:2880/accountfinish/";

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

    accountName = StringCodec(name);
    passphrase = StringCodec(pass);

    emit updateInfo("Generating account parameters");

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
    QString publicId = StringCodec(generator->getPublicId().toHexString());
    json["publicId"] = publicId;
    CK::PEMCodec codec(true);   // X509 key
    std::ostringstream pemstr;
    codec.encode(pemstr, *generator->getUserPublicKey());
    QString pem = StringCodec(pemstr.str());
    json["userPublicKey"] = pem;

    RESTHandler *handler = new RESTHandler;
    RESTTimer *timer = new RESTTimer(this);
    timer->addSource(handler, SIGNAL(requestComplete(RESTHandler*)));
    timer->addSource(handler, SIGNAL(requestFailed(RESTHandler*)));
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestFailed(RESTHandler*)));

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

void NewAccountCreator::doFinish() {

    QJsonObject json;

    json["sessionId"] = session->getSessionId();
    CK::RSACodec codec;
    codec << generator->getEnclaveKey();
    codec.encrypt(*generator->getServerPublicKey());
    QString encrypted = StringCodec(codec.toArray().toHexString());
    json["encrypted"] = encrypted;

    RESTHandler *handler = new RESTHandler;
    RESTTimer *timer = new RESTTimer(this);
    timer->addSource(handler, SIGNAL(requestComplete(RESTHandler*)));
    timer->addSource(handler, SIGNAL(requestFailed(RESTHandler*)));
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(finalComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestFailed(RESTHandler*)));

    handler->doPost(FINISH_URL, json);
    if (!timer->wait(30000)) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Request Error");
        message->setText("New account request timed out");
        message->setIcon(QMessageBox::Critical);
        message->exec();
        emit updateInfo("Unable to complete the request");
        qApp->processEvents();
    }

}

void NewAccountCreator::progress() {

    emit incrementProgress(10);
    qApp->processEvents();

}

void NewAccountCreator::finalComplete(RESTHandler *handler) {

    progress();
    QJsonObject json = handler->getResponse();

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Request Error");
        message->setText("An error occurred while processing the request");
        message->setInformativeText(value.toString());
        message->setIcon(QMessageBox::Critical);
        message->exec();
        emit updateInfo("Unable to complete the request");
        qApp->processEvents();
    }
    else {
        QJsonValue tokenValue = json["authToken"];
        QJsonValue keyValue = json["handlerKey"];
        if (tokenValue == QJsonValue::Null || keyValue == QJsonValue::Null) {
            QMessageBox *message = new QMessageBox;
            message->addButton(QMessageBox::Ok);
            message->setWindowTitle("Request Error");
            message->setText("An error occurred while processing the request");
            message->setInformativeText("Invalid server response");
            message->setIcon(QMessageBox::Critical);
            message->exec();
            emit updateInfo("Unable to complete the request");
            qApp->processEvents();
        }
        else {
            session->setAuthToken(tokenValue.toString());
            session->setHandlerKey(keyValue.toString());
            progress();
            emit updateInfo("New account complete");
            qApp->processEvents();
        }
    }
    handler->deleteLater();

}

void NewAccountCreator::requestComplete(RESTHandler *handler) {

    progress();
    QJsonObject json = handler->getResponse();

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Request Error");
        message->setText("An error occurred while processing the request");
        message->setInformativeText(value.toString());
        message->setIcon(QMessageBox::Critical);
        message->exec();
        emit updateInfo("Unable to complete the request");
        qApp->processEvents();
    }
    else {
        QJsonValue keyValue = json["serverPublicKey"];
        QJsonValue encValue = json["encrypted"];
        if (keyValue == QJsonValue::Null || encValue == QJsonValue::Null) {
            QMessageBox *message = new QMessageBox;
            message->addButton(QMessageBox::Ok);
            message->setWindowTitle("Request Error");
            message->setText("An error occurred while processing the request");
            message->setInformativeText("Invalid server response");
            message->setIcon(QMessageBox::Critical);
            message->exec();
            emit updateInfo("Unable to complete the request");
            qApp->processEvents();
        }
        else {
            generator->setServerPublicKey(StringCodec(keyValue.toString()));
            CK::RSACodec codec(coder::ByteArray(StringCodec(encValue.toString()), true));
            codec.decrypt(*generator->getUserPrivateKey());
            coder::ByteArray accountRandom;
            codec >> accountRandom;
            generator->setAccountRandom(accountRandom);
            progress();
            emit updateInfo("Finalizing the account");
            qApp->processEvents();
            doFinish();
        }
    }
    handler->deleteLater();

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
    handler->deleteLater();

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
