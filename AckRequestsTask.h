#ifndef ACKREQUESTSTASK_H
#define ACKREQUESTSTASK_H

#include "EnclaveRequestTask.h"
#include "ContactRequest.h"

namespace Pippip {

class AckRequestsTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        AckRequestsTask(SessionState *state, QObject *parent = 0);
        ~AckRequestsTask() {}

    signals:
        void ackRequestsComplete();
        void ackRequestsFailed(const QString& error);

    public:
        void acknowledgeRequests();
        void addAcknowledgement(qint64 requestId, const QString& ack);
        void clear();
        const RequestList& getRequestList() const { return requestList; }

    protected:
        void restComplete(const QJsonObject& resp);
        void restFailed(const QString& error);

    private:
        void setRequests(const QJsonArray& requests);

    private:
        RequestList requestList;

        struct AckRequest {
            qint64 requestId;
            QString ack;
        };
        typedef std::vector<AckRequest> AckList;
        AckList ackList;

};

}

#endif // ACKREQUESTSTASK_H
