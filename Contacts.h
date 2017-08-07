#ifndef CONTACTS_H
#define CONTACTS_H

#include "Contact.h"
#include <map>

namespace Pippip {

class Contacts {

    public:
        Contacts();
        ~Contacts() {}

    public:
        const Contact& operator [] (int contactId) const;
        const Contact& operator [] (const QString& nickname) const;

    public:
        void add(const Contact& contact);
        ContactList::const_iterator begin() const;
        void clear();
        /*
         * Create two difference lists. The lhs list will contain contacts not in
         * the input Contacts object. The rhs list will contain contacts not in
         * this object.
         */
        void diff(const Contacts& other, ContactList& lhs, ContactList& rhs) const;
        ContactList::const_iterator end() const;
        void fill(const Contacts& other);
        bool fromPublicId(const QString& publicId, Contact& contact) const;
        unsigned size() const;

    private:
        ContactList contactList;

        typedef std::map<int, Contact> ContactIdMap;
        typedef ContactIdMap::iterator IdIter;
        typedef ContactIdMap::const_iterator IdConstIter;

        ContactIdMap idMap;

        typedef std::map<QString, Contact> ContactMap;
        typedef ContactMap::iterator ContactIter;
        typedef ContactMap::const_iterator ContactConstIter;

        ContactMap publicIdMap;
        ContactMap nicknameMap;

};

}

#endif // CONTACTS_H
