#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "Message.h"
#include <QObject>
#include <deque>

class QByteArray;

namespace Pippip {

class SessionState;
class MessageDatabase;
class RESTHandler;
class ContactManager;
class NicknameManager;
struct Contact;
class Contacts;

class MessageManager : public QObject {
        Q_OBJECT

    public:
        explicit MessageManager(const QString& account, SessionState *state, QObject *parent = 0);
        ~MessageManager();

    signals:
        void messageSent();
        void requestFailed(const QString& reqName, const QString& error);

    private slots:
        void sendComplete(RESTHandler* handler);
        void sendTimedOut();

    public:
        void sendMessage(const QString& sender, const QString& recipient, const QString& message);
        void setContactManager(ContactManager *manager) { contactManager = manager; }

    private:
        void encryptMessage(const Contact& contact, Message& message, const QByteArray& msg);
        void sendMessage(const Message& message);

    private:
        bool timedOut;
        bool requestComplete;
        SessionState *state;
        MessageDatabase *database;
        ContactManager *contactManager;
        Contacts *contacts;
        NicknameManager *nicknameManager;
        typedef std::deque<Message> MessageQueue;
        MessageQueue messageQueue;

};

}

#endif // MESSAGEMANAGER_H
