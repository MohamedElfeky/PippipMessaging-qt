#ifndef CONTACT_H
#define CONTACT_H

#include "Entity.h"
#include <QStringList>

namespace Pippip {

struct Contact {
    QString status;
    QString contactOf;
    Entity entity;
    QStringList messageKeys;
    int currentKey;
};

typedef std::vector<Contact> ContactList;

}

#endif // CONTACT_H
