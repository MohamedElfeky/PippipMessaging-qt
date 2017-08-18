#ifndef REQUESTCONTACTTASK_H
#define REQUESTCONTACTTASK_H

#include "EnclaveRequestTask.h"
#include "ContactRequest.h"
#include "Contact.h"

namespace Pippip {

class RequestContactTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        RequestContactTask(SessionState *state, QObject *parent = 0);
        ~RequestContactTask() {}

    public:
        const ServerContact& getServerContact() const { return serverContact; }
        long getRequestId() const { return requestId; }
        const QString& getStatus() const { return status; }
        void setRequest(const ContactRequestOut& out);

    protected:
        bool requestComplete();

    private:
        void buildAndStoreContact();

    private:
        ContactRequestOut requestOut;
        ServerContact serverContact;
        long requestId;
        QString status;

};

}

#endif // REQUESTCONTACTTASK_H
