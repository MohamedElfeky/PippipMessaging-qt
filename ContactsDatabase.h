#ifndef CONTACTSDATABASE_H
#define CONTACTSDATABASE_H

#include "Contact.h"
#include <QObject>
#include <QSqlDatabase>

namespace Pippip {

class Contacts;
class SessionState;
class ContactManager;
struct ContactRequest;

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
        void requestContact(const ContactRequest& request);

    private:
        void addPending();
        bool contactExists(unsigned id);                    // Throws DatabaseException
        static void createDatabase(const QString& account); // Throws DatabaseException

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
