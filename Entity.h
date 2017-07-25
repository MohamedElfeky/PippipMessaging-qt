#ifndef ENTITY_H
#define ENTITY_H

#include <QString>
#include <vector>

namespace Pippip {

    struct Entity {
        QString nickname;
        QString publicId;
    };

    typedef std::vector<Entity> EntityList;

}

#endif // ENTITY_H
