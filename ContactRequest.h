#ifndef CONTACTREQUEST_H
#define CONTACTREQUEST_H

#include "Entity.h"

namespace Pippip {

struct ContactRequest {
    Entity requesting;
    Entity requested;
    bool privateRequest;
};

}

#endif // CONTACTREQUEST_H
