#ifndef PIPPIP_ENCLAVEREQUEST_H
#define PIPPIP_ENCLAVEREQUEST_H

#include "PostPacket.h"
#include <QString>
#include <map>

class QJsonObject;
class QJsonArray;
class QByteArray;

namespace Pippip {

class SessionState;

class EnclaveRequest : public PostPacket {

    public:
        EnclaveRequest(SessionState *state);
        ~EnclaveRequest();

    public:
        QString getValue(const QString& name) const;
        void setRequestType(const QString& type);
        void setLongValue(const QString& name, quint64 value);
        void setBoolValue(const QString& name, bool value);
        void setStringValue(const QString& name, const QString& value);
        void setObjectValue(const QString& name, const QJsonObject& value);
        void setArrayValue(const QString& name, const QJsonArray& value);
        void setBinaryValue(const QString& name, const QByteArray& value);

    public:
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    protected:
        SessionState *state;
        QJsonObject *request;

};

} // namespace Pippip

#endif // PIPPIP_ENCLAVEREQUEST_H
