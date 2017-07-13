#include "MessageManager.h"
#include "MessageDatabase.h"
#include "MessageException.h"
#include "EnclaveRequest.h"
#include "EnclaveResponse.h"
#include "RESTHandler.h"
#include "SessionState.h"
#include "ContactManager.h"
#include "ByteCodec.h"
#include <QTimer>

namespace Pippip {

MessageManager::MessageManager(const QString& account, SessionState *s, QObject *parent)
: QObject(parent),
  state(s),
  database(new MessageDatabase) {

    if (!database->open(account)) {
        throw MessageException("Unable to open message database");
    }

}

MessageManager::~MessageManager() {

    database->close();

}

void MessageManager::encryptMessage(const Contact& contact, Message &message, const QByteArray &msg) {

}

void MessageManager::sendComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestComplete = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed("SendMessage", handler->getError());
        }
        else if (response) {
            Message message = messageQueue.front();
            messageQueue.pop_front();
            QJsonValue tsValue = response.getValue("timestamp");
            QJsonValue idValue = response.getValue("requestId");
            if (idValue.isDouble() && tsValue.isDouble()) {
                message.timestamp = tsValue.toDouble();
                message.messageId = idValue.toDouble();
                if (database->addMessage(message)) {
                    emit messageSent();
                }
                else {
                    emit requestFailed("Send Message", "Message insert failed");
                }
            }
            else {
                emit requestFailed("Send Message", "Invalid server response");
            }
        }
        else {
            emit requestFailed("Send Message", response.getError());
        }
    }

}

void MessageManager::sendMessage(const Message &message) {

    messageQueue.push_back(message);

    EnclaveRequest request(state);
    request.setRequestType("sendMessage");
    request.setValue("sender", message.sender);
    request.setValue("senderId", state->publicId);
    request.setValue("recipient", message.recipient);
    request.setValue("recipientId", message.recipientId);
    request.setValue("keyIndex", message.keyIndex);
    request.setValue("message", message.message);

    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(sendComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(sendComplete(RESTHandler*)));
    timedOut = requestComplete = false;
    QTimer::singleShot(90000, this, SLOT(sendTimedOut()));
    handler->doPost(request);

}

void MessageManager::sendMessage(const QString &sender, const QString &recipient, const QString &message) {

    Message msg;
    msg.sender = sender;
    msg.recipient = recipient;

    Contact contact = contactManager->getContact(recipient);
    msg.recipientId = contact.entity.publicId;

    encryptMessage(contact, msg, message.toUtf8());
    sendMessage(msg);

}

void MessageManager::sendTimedOut() {

    if (!requestComplete) {
        timedOut = true;
        emit requestFailed("Send Message", "Request timed out");
    }

}

}
