#ifndef PIPPIP_ENCLAVEREQUEST_H
#define PIPPIP_ENCLAVEREQUEST_H

#include "PostPacket.h"
#include <QString>
#include <map>

class QJsonObject;

namespace Pippip {

class SessionState;

class EnclaveRequest : public PostPacket {

    public:
        EnclaveRequest(SessionState *state);
        ~EnclaveRequest();

    public:
        QString getValue(const QString& name) const;
        void setRequestType(const QString& type);
        void setValue(const QString& name, const QString& value);

    public:
        QJsonObject *getJson() const;
        const QString& getUrl() const;

    protected:
        SessionState *state;
        QJsonObject *request;

};

} // namespace Pippip

#endif // PIPPIP_ENCLAVEREQUEST_H
