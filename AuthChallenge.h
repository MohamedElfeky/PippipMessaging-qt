#ifndef PIPPIP_AUTHCHALLENGE_H
#define PIPPIP_AUTHCHALLENGE_H

#include "PostPacket.h"
#include <QJsonObject>
#include <coder/ByteArray.h>

namespace Pippip {

class SessionState;

class AuthChallenge : public PostPacket {

    public:
        AuthChallenge(SessionState *state);
        AuthChallenge(const QJsonObject& json, SessionState *state);
        ~AuthChallenge() {}

    private:
        AuthChallenge(const AuthChallenge& other);
        AuthChallenge& operator =(const AuthChallenge& other);

    public:
        operator bool();

    public:
        const QString& getError() const { return error; }
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    private:
        void generateClientHmac() const;
        void s2k() const;

    private:
        QJsonObject json;
        SessionState *state;
        QString error;
        mutable coder::ByteArray hmacKey;
        mutable coder::ByteArray hmac;
        mutable coder::ByteArray signature;

};

} // namespace Pippip

#endif // PIPPIP_AUTHCHALLENGE_H
