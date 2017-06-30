#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "Contact.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <vector>

class QJsonObject;

namespace Pippip {

struct SessionState;
struct ContactRequest;
class RESTHandler;

class ContactManager : public QObject {
        Q_OBJECT

    public:
        explicit ContactManager(QObject *parent, SessionState *state);
        ~ContactManager() {}

    signals:
        void requestedContact(Pippip::Contact);
        void contactsLoaded();
        void requestsLoaded();

    protected slots:
        void contactLoadComplete(RESTHandler*);
        void contactRequestComplete(RESTHandler*);
        void requestLoadComplete(RESTHandler*);
        void requestTimedOut();

    public:
        void requestContact(const ContactRequest& request);
        const ContactList& getContacts() const { return contacts; }
        void loadContacts();
        void loadRequests();

    private:
        bool loadContacts(const QJsonObject& json);

    private:
        SessionState *state;
        bool requestComplete;
        bool timedOut;

        ContactList contacts;

};

}

#endif // CONTACTMANAGER_H
