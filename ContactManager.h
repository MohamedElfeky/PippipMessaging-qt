#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "Contact.h"
#include <QObject>

class QJsonObject;
class QJsonArray;

namespace Pippip {

class SessionState;
struct ContactRequestOut;
class RESTHandler;
class Contacts;

class ContactManager : public QObject {
        Q_OBJECT

    public:
        explicit ContactManager(SessionState *state, QObject *parent = 0);
        ~ContactManager();

    signals:
        void addFailed(const QString& error);
        void contactRequestComplete(long, const QString&);
        void contactsLoaded();
        void deleteFailed(const QString& error);
        void loadFailed(const QString& error);
        void contactRequestFailed(const QString& error);

    protected slots:
        void addComplete(RESTHandler*);
        void addTimedOut();
        void contactLoadComplete(RESTHandler*);
        void deleteComplete(RESTHandler*);
        void deleteTimedOut();
        void contactRequestComplete(RESTHandler*);
        void loadTimedOut();
        void contactRequestTimedOut();

    public:
        void addContact(const Contact& contact);
        Contacts *getContacts() { return contacts; }
        void loadContacts();
        void reconcile(const Contacts& record);
        void requestContact(const ContactRequestOut& request);

    private:
        void addContacts();
        void deleteContacts();
        void encodeContacts(const ContactList& list, QJsonArray& array);
        bool loadContacts(const QJsonObject& json);

    private:
        SessionState *state;
        bool requestCompleted;
        bool timedOut;

        Contacts *contacts;
        ContactList toAdd;
        ContactList toDelete;

};

}

#endif // CONTACTMANAGER_H
