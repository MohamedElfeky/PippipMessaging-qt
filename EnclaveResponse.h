#ifndef PIPPIP_ENCLAVERESPONSE_H
#define PIPPIP_ENCLAVERESPONSE_H

#include <QJsonObject>

namespace Pippip {

struct SessionState;

class EnclaveResponse
{
    public:
        EnclaveResponse(const QJsonObject& json, SessionState *state);
        ~EnclaveResponse() {}

    private:
        EnclaveResponse(const EnclaveResponse& other);
        EnclaveResponse& operator =(const EnclaveResponse& other);

    public:
        operator bool();

    public:
        const QString& getError() const { return error; }
        const QJsonObject& getJson() const { return json; }

    private:
        QJsonObject encrypted;
        QJsonObject json;
        SessionState *state;
        QString error;

};

} // namespace Pippip

#endif // PIPPIP_ENCLAVERESPONSE_H
