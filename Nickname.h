#ifndef NICKNAME_H
#define NICKNAME_H

#include "Entity.h"

namespace Pippip {

    struct Nickname {
        Entity entity;
        QString policy;
        EntityList whitelist;
    };

    typedef std::vector<Nickname> NicknameList;

}

#endif // NICKNAME_H
