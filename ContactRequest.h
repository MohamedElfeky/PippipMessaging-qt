#ifndef CONTACTREQUEST_H
#define CONTACTREQUEST_H

#include "Entity.h"

namespace Pippip {

static const QString NICKNAME_NICKNAME = "NN";
static const QString NICKNAME_PUBLICID = "NP";
static const QString PUBLICID_NICKNAME = "PN";
static const QString PUBLICID_PUBLICID = "PP";

struct ContactRequest {
    QString idTypes;
    QString requestingId;
    QString requestedId;
};

typedef std::vector<ContactRequest> RequestList;

}

#endif // CONTACTREQUEST_H
