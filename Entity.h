#ifndef ENTITY_H
#define ENTITY_H

#include <QString>
#include <vector>

namespace Pippip {

    struct Entity {
        QString nickname;
        QString publicId;
        QString encryptionRSA;
        QString signingRSA;
    };

    typedef std::vector<Entity> EntityList;

}

#endif // ENTITY_H
