#ifndef PIPPIP_CLIENTAUTHORIZED_H
#define PIPPIP_CLIENTAUTHORIZED_H

#include <QJsonObject>

namespace Pippip {

class SessionState;

class ClientAuthorized {

    public:
        ClientAuthorized(const QJsonObject& json, SessionState *state);
        ~ClientAuthorized() {}

    private:
        ClientAuthorized(const ClientAuthorized& other);
        ClientAuthorized& operator =(const ClientAuthorized& other);

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

#endif // PIPPIP_CLIENTAUTHORIZED_H
