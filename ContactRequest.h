#ifndef CONTACTREQUEST_H
#define CONTACTREQUEST_H

#include "Entity.h"

namespace Pippip {

struct ContactRequest {
    Entity requesting;
    Entity requested;
    bool privateRequest;
};

typedef std::vector<ContactRequest> RequestList;

}

#endif // CONTACTREQUEST_H
