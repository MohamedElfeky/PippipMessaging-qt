#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "Contact.h"
#include <QObject>

class QJsonObject;
class QJsonArray;

namespace Pippip {

class SessionState;
struct ContactRequestOut;
struct ContactRequestIn;
class RESTHandler;
class Contacts;

class ContactManager : public QObject {
        Q_OBJECT

    public:
        explicit ContactManager(SessionState *state, QObject *parent = 0);
        ~ContactManager();

    signals:
        void contactRequestComplete(long, const QString&);
        void contactRequestFailed(const QString& error);
        void contactsLoaded();
        void deleteFailed(const QString& error);
        void loadFailed(const QString& error);
        void queryRequestComplete(const Pippip::ContactRequestIn& request);
        void queryRequestFailed(const QString& error);
        void updateContactFailed(const QString& error);

    protected slots:
        void contactLoadComplete(RESTHandler*);
        void contactRequestComplete(RESTHandler*);
        void contactRequestTimedOut();
        void deleteComplete(RESTHandler*);
        void deleteTimedOut();
        void loadTimedOut();
        void queryComplete(RESTHandler*);
        void queryTimedOut();
        //void updateContactComplete(RESTHandler*);
        //void updateContactTimedOut();

    public:
        Contacts *getContacts() { return contacts; }
        void loadContacts();
        void queryContact(long requestId, bool acknowledge);
        void reconcile(const Contacts& record);
        void requestContact(const ContactRequestOut& request);
        void updateContact(const Contact& contact);

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
