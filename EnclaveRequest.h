#ifndef PIPPIP_ENCLAVEREQUEST_H
#define PIPPIP_ENCLAVEREQUEST_H

#include "SessionTask.h"
#include <QJsonObject>
#include <QString>

namespace Pippip {

class EnclaveRequest : public SessionTask {
    Q_OBJECT

    public:
        EnclaveRequest(QObject *parent, SessionState *state);
        ~EnclaveRequest() {}

    public:
        QString getValue(const QString& name);
        void setRequestType(const QString& requestType);
        void setValue(const QString& name, const QString& value);

    protected:
        void sessionComplete();

    protected:
        QJsonObject request;
        QJsonObject response;

};

} // namespace Pippip

#endif // PIPPIP_ENCLAVEREQUEST_H
