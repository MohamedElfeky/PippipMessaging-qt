#ifndef CONTACTMANAGER_H
#define CONTACTMANAGER_H

#include "Contact.h"
#include <map>

namespace Pippip {

class SessionState;
class ContactsDatabase;
class Contacts;
struct ContactRequestIn;

class ContactManager {

    public:
        ContactManager(SessionState *state);
        ~ContactManager() {}

    public:
        void loadContacts(ContactsDatabase *contactsDb);
        void processContact(ContactRequestIn& request);
        void updateContact(ContactRequestIn& request);

    private:
        SessionState *state;
        ContactsDatabase *contactsDb;

        typedef std::map<qint64, quint32> RequestMap;
        typedef RequestMap::const_iterator ReqConstIter;
        RequestMap requestMap;

        typedef std::map<qint32, Contact> ContactMap;
        typedef ContactMap::iterator ContactIter;
        typedef ContactMap::const_iterator ContactConstIter;
        ContactMap contactMap;

};

}

#endif // CONTACTMANAGER_H
