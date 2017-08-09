#ifndef CONTACTSDATABASE_H
#define CONTACTSDATABASE_H

#include "Contact.h"
#include <QObject>
#include <QSqlDatabase>

namespace Pippip {

class Contacts;
class SessionState;
class ContactManager;
struct ContactRequestOut;
struct ContactRequestIn;
class ContactRequests;

class ContactsDatabase : public QObject {
        Q_OBJECT

    public:
        explicit ContactsDatabase(SessionState *state, QObject *parent = 0);
        ~ContactsDatabase() {}

    signals:
        void addContactFailed(const QString& error);
        void contactRequestComplete();
        void contactRequestFailed(const QString &error);
        void updateStatus(const QString& status);

    private slots:
        void contactRequestComplete(long requestId, const QString& status);
        void contactsLoaded();
        void loadFailed(const QString& error);

    public:
        void close();
        ContactManager *getContactManager() { return contactManager; }
        Contacts *getContacts() { return contacts; }
        static void initialize(const QString& account); // Throws DatabaseException
        void open(const QString& account);              // Throws DatabaseException
        void loadContacts();                            // Throws DatabaseException
        void requestsAcknowledged(ContactRequests *acknowledged);
        void requestContact(const ContactRequestOut& request);

    private:
        void addPending();
        bool contactExists(unsigned id);                    // Throws DatabaseException
        void createContact(Contact& contact, const ContactRequestIn& request);
        static void createDatabase(const QString& account); // Throws DatabaseException
        void newContact(const ContactRequestIn& request);
        void updateContact(const Contact& contact);
        void updateContact(Contact& contact, const ContactRequestIn& request);

    private:
        static bool once;
        SessionState *state;
        QSqlDatabase database;
        Contacts *contacts;
        ContactManager *contactManager;
        Contact pending;

};

}

#endif // CONTACTSDATABASE_H
