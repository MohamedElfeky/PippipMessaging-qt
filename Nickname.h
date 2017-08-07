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
        void clear() {
            entity.nickname = entity.publicId = "";
            rsaKeys.encryptionRSA = rsaKeys.signingRSA = "";
            policy = "";
            whitelist.clear();
        }
    };

    typedef std::vector<Nickname> NicknameList;

}

#endif // NICKNAME_H
