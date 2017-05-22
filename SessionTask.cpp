#include "SessionTask.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <QJsonObject>
#include <QJsonValue>

namespace Pippip {

static const QString SESSION_URL = "https://pippip.io:2880/io.pippip.rest/SessionRequest";

SessionTask::SessionTask(QObject *parent, SessionState *sess)
: QObject(parent),
  state(sess) {

    state->sessionState = SessionState::not_started;

}

SessionTask::~SessionTask() {

}

void SessionTask::errorResponse(const QString &error) {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("Request Error");
    message->setText("An error occurred while processing the request");
    message->setInformativeText(error);
    message->setIcon(QMessageBox::Critical);
    message->exec();
    qApp->processEvents();

}

void SessionTask::sessionResponse(RESTHandler *handler) {

    if (state->sessionState == SessionState::started) {
        QJsonObject json = handler->getResponse();
        QJsonValue value = json["error"];
        if (value != QJsonValue::Null) {
            errorResponse(value.toString());
            state->sessionState = SessionState::failed;
            sessionComplete("Unable to complete the request");
        }
        else {
            QJsonValue sessionId = json["sessionId"];
            if (sessionId.isDouble()) {
                state->sessionId = sessionId.toInt();
                state->sessionState = SessionState::established;
                sessionComplete("Success");
            }
            else {
                errorResponse("Invalid response from server");
                state->sessionState = SessionState::failed;
                sessionComplete("Unable to complete the request");
            }
        }
    }

}

void SessionTask::sessionFailed(RESTHandler *handler) {

    if (state->sessionState == SessionState::started) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Session Error");
        message->setText("An error occurred while establishing a session with the server.");
        message->setInformativeText(handler->getError());
        message->setIcon(QMessageBox::Critical);
        message->exec();
        state->sessionState = SessionState::failed;
        sessionComplete("Unable to establish a session with the server");
    }

}

void SessionTask::sessionTimedOut() {

    if (state->sessionState != SessionState::established
                            && state->sessionState != SessionState::failed) {
        QMessageBox *message = new QMessageBox;
        message->addButton(QMessageBox::Ok);
        message->setWindowTitle("Session Error");
        message->setText("Session request timed out");
        message->setIcon(QMessageBox::Critical);
        message->exec();
        state->sessionState = SessionState::failed;
        sessionComplete("Unable to establish a session with the server");
    }

}

void SessionTask::startSession() {

    if (state->sessionState == SessionState::not_started) {
        QTimer::singleShot(10000, this, SLOT(sessionTimedOut()));
        RESTHandler *handler = new RESTHandler(this);
        connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(sessionResponse(RESTHandler*)));
        connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(sessionFailed(RESTHandler*)));
        state->sessionState = SessionState::started;
        handler->doGet(SESSION_URL);
    }

}

}
