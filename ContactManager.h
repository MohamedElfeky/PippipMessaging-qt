#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "Contact.h"
#include "ContactRequest.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <vector>

class QJsonObject;

namespace Pippip {

class SessionState;
struct ContactRequest;
class RESTHandler;
struct Message;

class ContactManager : public QObject {
        Q_OBJECT

    public:
        explicit ContactManager(QObject *parent, SessionState *state);
        ~ContactManager() {}

    signals:
        void contactRequested(const QString&);
        void contactsLoaded();
        void requestFailed(const QString& reqName, const QString& error);
        void requestsLoaded();

    protected slots:
        void contactLoadComplete(RESTHandler*);
        void contactRequestComplete(RESTHandler*);
        void requestLoadComplete(RESTHandler*);
        void requestTimedOut();

    public:
        bool getContactByNickname(const QString& nickname, Contact& contact) const;
        bool getContactById(const QString& id, Contact& contact) const;
        const Contact& getContactById(const QString& id) const;
        const ContactList& getContacts() const { return contacts; }
        const RequestList& getRequests() const { return requests; }
        void incrementSequences(const Message& message);
        void loadContacts();
        void loadRequests();
        void requestContact(const ContactRequest& request);

    private:
        bool loadContacts(const QJsonObject& json);

    private:
        SessionState *state;
        bool requestComplete;
        bool timedOut;

        ContactList contacts;
        RequestList requests;

};

}

#endif // CONTACTMANAGER_H
