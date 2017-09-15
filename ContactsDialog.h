#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "Contact.h"
//#include "Nickname.h"
#include <QDialog>

namespace Ui {
class ContactsDialog;
}

namespace Pippip {
    class ContactDirector;
    class ContactRequests;
    class SessionState;
    class LoadNicknamesTask;
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
        void loadNicknamesFailed(const QString& error);
        void nicknamesLoaded();

    private slots:
        void contactSelected();
        void contactsModified();
        void tabSelected(int tab);
        void reloadNicknames();
        void updateStatus(const QString& icon, const QString& status);

    public:
        int exec();
        void requestsAcknowledged(Pippip::ContactRequests *acknowledged);
        void setContactDirector(Pippip::ContactDirector *director);

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        ContactHandler *contactHandler;
        RequestHandler *requestHandler;
        Pippip::ContactDirector *contactDirector;
        Pippip::LoadNicknamesTask *loadNicknamesTask;

};

#endif // CONTACTSDIALOG_H
