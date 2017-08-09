#ifndef CONTACTREQUEST_H
#define CONTACTREQUEST_H

#include "Entity.h"
#include "RSAKeys.h"
#include <coder/ByteArray.h>

namespace Pippip {

struct ContactRequestOut {
    QString idTypes;
    QString requestingId;
    QString requestedId;
};

struct ContactRequestIn {
    QString status;
    long requestId;
    Entity requested;
    Entity requesting;
    RSAKeys rsaKeys;
    coder::ByteArray keyBlock;
};

typedef std::vector<ContactRequestIn> RequestList;

}

#endif // CONTACTREQUEST_H
