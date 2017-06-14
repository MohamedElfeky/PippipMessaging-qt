#ifndef CONTACT_H
#define CONTACT_H

#include <QString>
#include <QStringList>

namespace Pippip {

struct Contact {
    QString status;
    QString nickname;
    QString publicId;
    QString encryptionRSA;
    QString signingRSA;
    QStringList keys;
};

typedef std::vector<Contact> ContactList;

}

#endif // CONTACT_H
