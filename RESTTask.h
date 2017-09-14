#ifndef RESTTASK_H
#define RESTTASK_H

#include <QObject>

namespace Pippip {

class RESTHandler;
class PostPacket;

class RESTTask : public QObject {
        Q_OBJECT

    protected:
        RESTTask(QObject *parent);

    public:
        ~RESTTask() {}

    signals:

    public slots:
        void requestComplete(RESTHandler *handler);
        void requestFailed(RESTHandler *handler);
        void requestTimedOut();

    protected:
        void doRESTPost(int timeout, const PostPacket& packet);
        virtual void restComplete(const QJsonObject& obj)=0;
        virtual void restFailed(const QString& error)=0;
        virtual void restTimedOut()=0;

    protected:
        bool timedOut;
        bool completed;

};

}

#endif // RESTTASK_H
