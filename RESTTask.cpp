#include "RESTTask.h"
#include "RESTHandler.h"
//#include "PostPacket.h"
#include <QTimer>

namespace Pippip {

RESTTask::RESTTask(QObject *parent)
: QObject(parent) {

}

void RESTTask::doRESTPost(int timeout, const PostPacket &packet) {

    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestFailed(RESTHandler*)));
    timedOut = completed = false;
    QTimer::singleShot(timeout * 1000, this, SLOT(requestTimedOut()));
    handler->doPost(packet);

}

void RESTTask::requestComplete(RESTHandler *handler) {

    if (!timedOut) {
        completed = true;
        restComplete(handler->getResponse());
    }

}

void RESTTask::requestFailed(RESTHandler *handler) {

    if (!timedOut) {
        completed = true;
        restFailed(handler->getError());
    }

}

void RESTTask::requestTimedOut() {

    if (!completed) {
        timedOut = true;
        restTimedOut();
    }

}

}
