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
#include "ParameterGenerator.h"
#include "RESTHandler.h"
#include "NewAccountRequest.h"
#include "NewAccountResponse.h"
#include "NewAccountFinish.h"
#include "NewAccountFinal.h"
#include "Vault.h"
#include "VaultException.h"
#include "MessageDatabase.h"
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <sstream>
#include <memory>

namespace Pippip {

NewAccountCreator::NewAccountCreator(NewAccountDialog *parent, ParameterGenerator *gen)
: SessionTask(parent, gen),
  timedOut(false),
  responseComplete(false),
  finalComplete(false),
  dialog(parent),
  generator(gen) {

    connect(this, SIGNAL(accountComplete(int)), dialog, SLOT(done(int)));
    connect(this, SIGNAL(incrementProgress(int)), dialog, SLOT(incrementProgress(int)));
    connect(this, SIGNAL(updateInfo(QString)), dialog, SLOT(updateInfo(QString)));
    connect(this, SIGNAL(resetProgress()), dialog, SLOT(resetProgress()));

}

NewAccountCreator::~NewAccountCreator() {
}

void NewAccountCreator::addAccount(const QString& newUser) {

    QSettings settings;
    QByteArray json = settings.value("User/accounts").value<QByteArray>();
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject accountsObj = doc.object();
    QJsonArray accounts = accountsObj["users"].toArray();
    accounts.append(newUser);
    QJsonObject newAccounts;
    newAccounts["users"] = accounts;
    QJsonDocument newDoc(newAccounts);
    QString newjson = QString(newDoc.toJson());
    settings.setValue("User/accounts", newDoc.toJson());

}


void NewAccountCreator::createNewAccount(const QString &name, const QString &pass) {

    accountName = name;
    passphrase = pass;

    emit updateInfo("Generating account parameters");

    generator->generateParameters(accountName.toUtf8().toStdString());

    emit incrementProgress(20);
    emit updateInfo("Contacting the server");
    startSession();

}

void NewAccountCreator::doRequest() {

    emit incrementProgress(20);
    emit updateInfo("Starting the request");
    NewAccountRequest req(state);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    QTimer::singleShot(20000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

void NewAccountCreator::doFinish() {

    emit incrementProgress(20);
    emit updateInfo("Finalizing the account");
    NewAccountFinish finish(state);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(finishComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(finishComplete(RESTHandler*)));
    QTimer::singleShot(90000, this, SLOT(finishTimedOut()));
    handler->doPost(finish);

}

void NewAccountCreator::finishComplete(RESTHandler *handler) {

    if (!timedOut) {
        finalComplete = true;
        emit incrementProgress(10);

        NewAccountFinal final(handler->getResponse(), state);
        if (!handler->successful()) {
            requestFailed(handler->getError());
        }
        else if (final) {
            if (MessageDatabase::create(accountName)) {
                try {
                    std::unique_ptr<Vault> vault(new Vault(*state));
                    vault->storeVault(accountName, passphrase);
                    addAccount(accountName);
                    emit accountComplete(0);
                }
                catch (VaultException& e) {
                    requestFailed(QString(e.what()));
                }
            }
            else {
                requestFailed("Failed to create message database");
            }
        }
        else {
            requestFailed(final.getError());
        }
    }

}

void NewAccountCreator::finishTimedOut() {

    if (!finalComplete) {
        timedOut = true;
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Account Request Error");
        message->setText("Account request timed out");
        message->setIcon(QMessageBox::Critical);
        message->exec();
        emit updateInfo("Request timed out");
        emit resetProgress();
    }

}

void NewAccountCreator::requestComplete(RESTHandler *handler) {

    if (!timedOut) {
        responseComplete = true;
        emit incrementProgress(10);

        NewAccountResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            requestFailed(handler->getError());
        }
        else if (response) {
            doFinish();
        }
        else {
            requestFailed(response.getError());
        }
    }

}

void NewAccountCreator::requestFailed(const QString& error) {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("Request Error");
    message->setText("An error occurred while processing the request");
    message->setInformativeText(error);
    message->setIcon(QMessageBox::Critical);
    message->exec();
    emit updateInfo("Unable to complete the Request");
    emit resetProgress();

}

void NewAccountCreator::requestTimedOut() {

    if (!responseComplete) {
        timedOut = true;
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Account Request Error");
        message->setText("Account request timed out");
        message->setIcon(QMessageBox::Critical);
        message->exec();
        emit updateInfo("Request timed out");
        emit resetProgress();
    }

}

void NewAccountCreator::sessionComplete(const QString& result) {

    if (state->sessionState == SessionState::established) {
        emit updateInfo("Session started");
        emit incrementProgress(20);
        doRequest();
    }
    else {
        emit updateInfo(result);
        emit resetProgress();
    }

}

}
