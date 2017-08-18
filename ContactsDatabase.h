#ifndef CONTACTSDATABASE_H
#define CONTACTSDATABASE_H

#include "Contact.h"
#include <QSqlDatabase>

namespace Pippip {
/*
class EnclaveRequestTask;
class Contacts;
struct ContactRequestOut;
struct ContactRequestIn;
class ContactRequests;
*/
class SessionState;

class ContactsDatabase {

    protected:
        ContactsDatabase(SessionState *state);

    public:
        ~ContactsDatabase() {}

    public:
        void addContact(DatabaseContact& contact);
        void addContacts(const DatabaseContactList& list);
        void close();
        static void createDatabase(const QString& account);
        void deleteContact(unsigned id);
        void getContacts(DatabaseContactList& list);
        static void initialize(SessionState *state);
        static ContactsDatabase *open(SessionState *state);
        void updateContact(const DatabaseContact& contact);

    private:
        bool contactExists(unsigned id);
/*        void createContact(Contact& contact, const ContactRequestIn& request);
        void insertContacts(const ServerContactList& contacts);
        void loadServerContacts();
        void newContact(const ContactRequestIn& request);
        void resolveContacts(const ServerContactList& serverContacts);
        void updateContact(const Contact& contact);
        void updateContact(Contact& contact, const ContactRequestIn& request);
*/
    private:
        static bool once;
        SessionState *state;
        QSqlDatabase database;

};

}

#endif // CONTACTSDATABASE_H
