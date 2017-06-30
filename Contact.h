#ifndef CONTACT_H
#define CONTACT_H

#include "Entity.h"
#include <QStringList>

namespace Pippip {

struct Contact {
    QString status;
    Entity entity;
    QStringList messageKeys;
};

typedef std::vector<Contact> ContactList;

}

#endif // CONTACT_H
