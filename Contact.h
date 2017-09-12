#ifndef CONTACT_H
#define CONTACT_H

#include "Entity.h"
#include "RSAKeys.h"
#include <QStringList>
#include <QByteArray>
#include <coder/ByteStreamCodec.h>

namespace Pippip {

static const QString CURRENT_VERSION = "1.0";

struct Contact {
    Contact() : version(CURRENT_VERSION), contactId(0), currentKey(0),
                currentSequence(0) {}
    QString version;
    QString status;
    unsigned contactId;
    long long requestId;    // Der? This is the 64 bit integer type.
    QString contactOf;
    Entity entity;
    RSAKeys rsaKeys;
    coder::ByteArray nonce;
    coder::ByteArray authData;
    std::vector<coder::ByteArray> messageKeys;
    int currentKey;
    int currentSequence;
};

typedef std::vector<Contact> ContactList;

}

#endif // CONTACT_H
