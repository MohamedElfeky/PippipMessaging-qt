/*
 * Authenticator.cpp
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

#include "Authenticator.h"
#include "AuthRequest.h"
#include "AuthResponse.h"
#include "AuthChallenge.h"
#include "ClientAuthorized.h"
#include "ServerAuthorized.h"
#include "Vault.h"
#include "VaultException.h"
#include "LoginDialog.h"
#include "RESTHandler.h"
#include "CriticalAlert.h"
#include <QMessageBox>
#include <QTimer>
#include <string>

namespace Pippip {

Authenticator::Authenticator(LoginDialog *parent, Vault *v)
: SessionTask(parent, v),
  responseComplete(false),
  challengeCompleted(false),
  timedOut(false),
  dialog(parent),
  vault(v) {

    connect(this, SIGNAL(authenticationComplete(int)), dialog, SLOT(done(int)));
    connect(this, SIGNAL(updateInfo(QString)), dialog, SLOT(updateInfo(QString)));

}

Authenticator::~Authenticator() {
}

void Authenticator::authenticate(const QString &name, const QString &pass) {

    std::string accountName = name.toStdString();
    std::string passphrase = pass.toStdString();

    emit updateInfo("Opening user vault");

    try {
        vault->loadVault(accountName, passphrase);
        emit updateInfo("Contacting the server");
        startSession();
    }
    catch (Pippip::VaultException& e) {
        authFailed(e.what());
    }

}

void Authenticator::authFailed(const QString& error) {

    CriticalAlert alert("Authentication Failed",
                        "An error occurred while processing the authentication",
                        error);
    alert.exec();
    emit updateInfo("Unable to authenticate");

}

void Authenticator::authorizeComplete(RESTHandler *handler) {

    if (!timedOut) {
        authComplete = true;

        ClientAuthorized auth(handler->getResponse(), state);
        if (!handler->successful()) {
            authFailed(handler->getError());
        }
        else if (auth) {
            emit updateStatus("Authentication Complete");
            emit authenticationComplete(1);
        }
        else {
            authFailed(auth.getError());
        }
    }

}

void Authenticator::authorizeTimedOut() {

    if (!authComplete) {
        timedOut = true;
        CriticalAlert alert("Login Request Error",
                            "Login request timed out");
        alert.exec();
        emit updateInfo("Login request timed out");
    }

}

void Authenticator::challengeComplete(RESTHandler *handler) {

    if (!timedOut) {
        challengeCompleted = true;

        AuthChallenge challenge(handler->getResponse(), state);
        if (!handler->successful()) {
            authFailed(handler->getError());
        }
        else if (challenge) {
            doAuthorized();
        }
        else {
            authFailed(challenge.getError());
        }
    }

}

void Authenticator::challengeTimedOut() {

    if (!challengeCompleted) {
        timedOut = true;
        CriticalAlert alert("Login Request Error",
                            "Login request timed out");
        alert.exec();
        emit updateInfo("Login request timed out");
    }

}

void Authenticator::doAuthorized() {

    emit updateInfo("Finishing authentication");
    ServerAuthorized auth(state);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(authorizeComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(authorizeComplete(RESTHandler*)));
    QTimer::singleShot(20000, this, SLOT(authorizeTimedOut()));
    handler->doPost(auth);

}

void Authenticator::doChallenge() {

    emit updateInfo("Issuing authentication challenge");
    AuthChallenge challenge(state);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(challengeComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(challengeComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(challengeTimedOut()));
    handler->doPost(challenge);

}

void Authenticator::doLogin() {

    emit updateInfo("Starting authentication");
    AuthRequest req(state);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

void Authenticator::requestComplete(RESTHandler *handler) {

    if (!timedOut) {
        responseComplete = true;

        AuthResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            authFailed(handler->getError());
        }
        else if (response) {
            doChallenge();
        }
        else {
            authFailed(response.getError());
        }
    }

}

void Authenticator::requestTimedOut() {

    if (!responseComplete) {
        timedOut = true;
        CriticalAlert alert("Login Request Error",
                            "Login request timed out");
        alert.exec();
        emit updateInfo("Login request timed out");
    }

}

void Authenticator::reset() {

    responseComplete = false;
    challengeCompleted = false;
    timedOut = false;
    state->sessionState = SessionState::not_started;

}

void Authenticator::sessionComplete(const QString& result) {

    if (state->sessionState == SessionState::established) {
        emit updateInfo("Session started");
        doLogin();
    }
    else {
        emit updateInfo(result);
    }

}

}
