#ifndef SESSIONTASK_H
#define SESSIONTASK_H

#include <QObject>

class QString;

namespace Pippip {

struct SessionState;
class RESTHandler;

class SessionTask : public QObject {
    Q_OBJECT

    public:
        explicit SessionTask(QObject *parent, SessionState *state);
        ~SessionTask();

    public slots:
        void sessionResponse(RESTHandler*);
        void sessionFailed(RESTHandler*);
        void sessionTimedOut();

    protected:
        void errorResponse(const QString& error);
        virtual void sessionComplete(const QString& result)=0;
        void startSession();

    protected:
        SessionState *state;

};

}

#endif // SESSIONTASK_H
