#include "Contacts.h"
#include "StringCodec.h"

namespace Pippip {

static const Contact EMPTY;

Contacts::Contacts() {

}

const Contact& Contacts::operator [] (int contactId) const {

    IdConstIter it = idMap.find(contactId);
    if (it != idMap.end()) {
        return it->second;
    }
    else {
        return EMPTY;
    }

}

const Contact& Contacts::operator [] (const QString& nickname) const {

    ContactConstIter it = nicknameMap.find(nickname);
    if (it != nicknameMap.end()) {
        return it->second;
    }
    else {
        return EMPTY;
    }

}

void Contacts::add(const Contact &contact) {

    contactList.push_back(contact);
    idMap[contact.contactId] = contact;
    if (contact.entity.nickname.length() > 0) {
        nicknameMap[contact.entity.nickname] = contact;
    }
    if (contact.entity.publicId.length() > 0) {
        publicIdMap[contact.entity.publicId] = contact;
    }

}

ContactList::const_iterator Contacts::begin() const {

    return contactList.begin();

}

void Contacts::clear() {

    contactList.clear();
    idMap.clear();
    publicIdMap.clear();
    nicknameMap.clear();

}

/*
 * Create two difference lists. The lhs list will contain contacts not in
 * the input Contacts object. The rhs list will contain contacts not in
 * this object.
 */
void Contacts::diff(const Contacts &other, ContactList &lhs, ContactList &rhs) const {

    for (auto contact : contactList) {
        IdConstIter it = other.idMap.find(contact.contactId);
        if (it != other.idMap.end()) {
            lhs.push_back(contact);
        }
    }

    for (auto contact : other.contactList) {
        IdConstIter it = idMap.find(contact.contactId);
        if (it != idMap.end()) {
            rhs.push_back(contact);
        }
    }

}

ContactList::const_iterator Contacts::end() const {

    return contactList.end();

}

void Contacts::fill(const Contacts &other) {

    clear();
    for (auto pair : other.idMap) {
        add(pair.second);
    }

}

unsigned Contacts::size() const {

    return idMap.size();

}

}
