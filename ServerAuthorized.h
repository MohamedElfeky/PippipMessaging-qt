#ifndef PIPPIP_SERVERAUTHORIZED_H
#define PIPPIP_SERVERAUTHORIZED_H

#include "PostPacket.h"

namespace Pippip {

struct SessionState;

class ServerAuthorized : public PostPacket {

    public:
        ServerAuthorized(SessionState *state);
        ~ServerAuthorized() {}

    private:
        ServerAuthorized(const ServerAuthorized& other);
        ServerAuthorized& operator =(const ServerAuthorized& other);

    public:
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    private:
        SessionState *state;

};

} // namespace Pippip

#endif // PIPPIP_SERVERAUTHORIZED_H
