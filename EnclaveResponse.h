#ifndef PIPPIP_ENCLAVERESPONSE_H
#define PIPPIP_ENCLAVERESPONSE_H

#include <QJsonObject>
#include <QJsonValue>

namespace Pippip {

class SessionState;

class EnclaveResponse {

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
        const QString& getResponseType() const { return responseType; }
        QJsonArray getResponseArray(const QString& name) const;
        qint64 getResponseLong(const QString& name) const;
        QString getResponseString(const QString& name) const;
        QJsonValue getResponseValue(const QString& name) const;
        const QJsonObject& getJson() const { return json; }

    private:
        // QJsonObject encrypted;
        QJsonObject json;
        SessionState *state;
        QString responseType;
        QString error;

};

} // namespace Pippip

#endif // PIPPIP_ENCLAVERESPONSE_H
