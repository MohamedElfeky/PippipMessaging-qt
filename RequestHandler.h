#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "ContactRequest.h"
#include <QObject>

namespace Ui {
    class ContactsDialog;
}

class RequestHandler : public QObject {
        Q_OBJECT
    public:
        explicit RequestHandler(Ui::ContactsDialog *ui, QObject *parent = 0);
        ~RequestHandler() {}

    signals:

    public slots:

    public:
        void setRequests(const Pippip::RequestList& requests);

    private:
        Ui::ContactsDialog *ui;
        Pippip::RequestList requests;

};

#endif // REQUESTHANDLER_H
