#ifndef NICKNAME_H
#define NICKNAME_H

#include "Entity.h"
#include "RSAKeys.h"

namespace Pippip {

    struct Nickname {
        Entity entity;
        RSAKeys rsaKeys;
        QString policy;
        EntityList whitelist;
    };

    typedef std::vector<Nickname> NicknameList;

}

#endif // NICKNAME_H
