#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include <QObject>

class QJsonObject;

namespace Pippip {

class SessionState;
class ContactRequests;
class RESTHandler;

class RequestManager : public QObject {
        Q_OBJECT

    public:
        explicit RequestManager(SessionState *state, QObject *parent = 0);
        ~RequestManager();

    signals:
        void ackFailed(const QString& error);
        void loadFailed(const QString& error);
        void requestsAcknowledged();
        void requestsLoaded();

    private slots:
        void ackComplete(RESTHandler *handler);
        void ackTimedOut();
        void requestLoadComplete(RESTHandler *handler);
        void loadTimedOut();

    public:
        void ackRequest(const QString& ack, long requestId);
        ContactRequests *getAcknowledged() { return acknowledged; }
        ContactRequests *getContactRequests() { return contactRequests; }
        void loadRequests();

    private:
        bool loadAcknowledged(const QJsonObject& json);
        bool loadRequests(const QJsonObject& json);

    private:
        bool loaded;
        bool timedOut;
        bool requestCompleted;
        SessionState *state;
        ContactRequests *contactRequests;
        ContactRequests *acknowledged;

};

}

#endif // REQUESTMANAGER_H
