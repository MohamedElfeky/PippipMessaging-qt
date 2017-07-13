#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "Contact.h"
#include "Nickname.h"
#include <QDialog>

namespace Ui {
class ContactsDialog;
}

namespace Pippip {
    class ContactManager;
    class NicknameManager;
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
        void requestFailed(const QString& reqName, const QString& error);
        //void requestsLoaded();

    private slots:
        void contactsLoaded();

    public:
        void setContactManager(Pippip::ContactManager *manager);
        void setNicknameManager(Pippip::NicknameManager *manager);

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        Pippip::ContactManager *contactManager;
        Pippip::NicknameManager *nicknameManager;
        ContactHandler *contactHandler;
        RequestHandler *requestHandler;

};

#endif // CONTACTSDIALOG_H
