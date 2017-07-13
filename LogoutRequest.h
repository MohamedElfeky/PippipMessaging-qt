#ifndef LOGOUTREQUEST_H
#define LOGOUTREQUEST_H

#include "PostPacket.h"

namespace Pippip {

class SessionState;

class LogoutRequest : public Pippip::PostPacket {

    public:
        LogoutRequest(SessionState *state);
        ~LogoutRequest() {}

    public:
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    private:
        SessionState *state;

};

} // namespace Pippip

#endif // LOGOUTREQUEST_H
