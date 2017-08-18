#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "Contact.h"
//#include "Nickname.h"
#include <QDialog>

namespace Ui {
class ContactsDialog;
}

namespace Pippip {
    class ContactsDatabase;
    class ContactRequests;
    class SessionState;
}

class ContactHandler;
class RequestHandler;

class ContactsDialog : public QDialog {
        Q_OBJECT

    public:
        explicit ContactsDialog(Pippip::SessionState *state, QWidget *parent = 0);
        ~ContactsDialog();

    public slots:
        //void contactRequested(Pippip::Contact);
        void nicknamesLoaded();

    private slots:
        void contactSelected();
        void contactsModified();
        void tabSelected(int tab);
        void updateStatus(const QString& icon, const QString& status);

    public:
        int exec();
        void requestsAcknowledged(Pippip::ContactRequests *acknowledged);
        void setContactsDatabase(Pippip::ContactsDatabase *database);

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        Pippip::ContactsDatabase *contactsDatabase;
        ContactHandler *contactHandler;
        RequestHandler *requestHandler;

};

#endif // CONTACTSDIALOG_H
