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
class RESTHandler;

class ContactManager : public QObject {
        Q_OBJECT

    public:
        explicit ContactManager(QObject *parent, SessionState *state);
        ~ContactManager() {}

    signals:
        void addedContact(Pippip::Contact);
        void contactsLoaded();
        void requestsLoaded();

    protected slots:
        void contactAdded(RESTHandler*);
        void contactLoadComplete(RESTHandler*);
        void requestLoadComplete(RESTHandler*);
        void requestTimedOut();

    public:
        void addContact(const Contact& contact);
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
