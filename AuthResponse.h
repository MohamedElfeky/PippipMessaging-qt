#ifndef PIPPIP_AUTHRESPONSE_H
#define PIPPIP_AUTHRESPONSE_H

#include <QJsonObject>

namespace Pippip {

class SessionState;

class AuthResponse {
    public:
        AuthResponse(const QJsonObject& json, SessionState *state);
        ~AuthResponse();

    private:
        AuthResponse(const AuthResponse& other);
        AuthResponse& operator =(const AuthResponse& other);

    public:
        operator bool();

    public:
        const QString& getError() const { return error; }

    private:
        QJsonObject json;
        SessionState *state;
        QString error;

};

} // namespace Pippip

#endif // PIPPIP_AUTHRESPONSE_H
