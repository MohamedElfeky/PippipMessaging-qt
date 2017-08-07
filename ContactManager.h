#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "Contact.h"
#include <QObject>
//#include <QStringList>

class QJsonObject;
class QJsonArray;

namespace Pippip {

class SessionState;
struct ContactRequest;
class RESTHandler;
//struct Message;
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
        //void requestsLoaded();

    protected slots:
        void addComplete(RESTHandler*);
        void addTimedOut();
        void contactLoadComplete(RESTHandler*);
        void deleteComplete(RESTHandler*);
        void deleteTimedOut();
        void contactRequestComplete(RESTHandler*);
        //void requestLoadComplete(RESTHandler*);
        void loadTimedOut();
        void contactRequestTimedOut();

    public:
        void addContact(const Contact& contact);
        //bool getContactByNickname(const QString& nickname, Contact& contact) const;
        //bool getContactById(const QString& id, Contact& contact) const;
        //const Contact& getContactById(const QString& id) const;
        Contacts *getContacts() { return contacts; }
        //const RequestList& getRequests() const { return requests; }
        //void incrementSequences(const Message& message);
        void loadContacts();
        //void loadRequests();
        void reconcile(const Contacts& record);
        void requestContact(const ContactRequest& request);

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
