#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "ContactRequest.h"
#include <QObject>

namespace Ui {
    class ContactsDialog;
}

namespace Pippip {
    class ContactRequests;
    class RequestManager;
    class SessionState;
}

class ContactsDialog;

class RequestHandler : public QObject {
        Q_OBJECT

    public:
        explicit RequestHandler(Ui::ContactsDialog *ui, Pippip::SessionState *state, ContactsDialog *parent = 0);
        ~RequestHandler() {}

    signals:

    public slots:
        void ackFailed(const QString& error);
        void loadFailed(const QString& error);
        void requestsLoaded();

    private slots:
        void acceptContact();

    private:
        void loadTable();

    public:
        Pippip::ContactRequests *getAcknowledged() { return acknowledged; }
        void loadRequests();

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        Pippip::RequestManager *requestManager;
        Pippip::ContactRequests *requests;
        Pippip::ContactRequests *acknowledged;

};

#endif // REQUESTHANDLER_H
