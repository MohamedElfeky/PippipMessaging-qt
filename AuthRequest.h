#ifndef PIPPIP_AUTHREQUEST_H
#define PIPPIP_AUTHREQUEST_H

#include "PostPacket.h"

namespace Pippip {

class SessionState;

class AuthRequest : public PostPacket {
    public:
        AuthRequest(SessionState *state);
        ~AuthRequest() {}

    public:
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    private:
        SessionState *state;

};

} // namespace Pippip

#endif // PIPPIP_AUTHREQUEST_H
