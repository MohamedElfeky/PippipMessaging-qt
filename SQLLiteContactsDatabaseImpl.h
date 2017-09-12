#ifndef SQLLITECONTACTSDATABASEIMPL_H
#define SQLLITECONTACTSDATABASEIMPL_H

#include "ContactsDatabase.h"
#include <QSqlDatabase>

namespace Pippip {

class SQLLiteContactsDatabaseImpl : public ContactsDatabase {

    protected:
        friend class DatabaseImpl;
        SQLLiteContactsDatabaseImpl();

    public:
        ~SQLLiteContactsDatabaseImpl() {}

    public:
        void addContact(const Contact& contact, SessionState *state);
        void close();
        void create(const QString& account);
        int getNextContactId();
        void getContacts(ContactList& list, SessionState *state);
        void open(const QString& account);
        void updateContact(const Contact& contact, SessionState *state);

    private:
        bool contactExists(int contactId);
        void initialize();

    private:
        static bool once;
        bool closed;
        QSqlDatabase database;

};

}

#endif // SQLLITECONTACTSDATABASEIMPL_H
