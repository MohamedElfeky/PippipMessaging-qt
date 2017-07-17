#ifndef CONTACT_H
#define CONTACT_H

#include "Entity.h"
#include <QStringList>

namespace Pippip {

struct Contact {
    QString status;
    QString contactOf;
    Entity entity;
    QString nonce;
    QString authData;
    QStringList messageKeys;
    int currentKey;
    int currentSequence;
};

typedef std::vector<Contact> ContactList;

}

#endif // CONTACT_H
