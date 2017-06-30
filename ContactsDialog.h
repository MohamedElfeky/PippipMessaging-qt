#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "Contact.h"
#include <QDialog>

namespace Ui {
class ContactsDialog;
}

namespace Pippip {
    class ContactManager;
    class NicknameManager;
    struct SessionState;
}

class ContactsDialog : public QDialog {
        Q_OBJECT

    public:
        explicit ContactsDialog(Pippip::SessionState *state, QWidget *parent = 0);
        ~ContactsDialog();

    public slots:
        void contactRequested(Pippip::Contact);
        void contactsLoaded();
        void requestsLoaded();

    private slots:
        void requestContact();

    public:
        void setContactManager(Pippip::ContactManager *manager);
        void setNicknameManager(Pippip::NicknameManager *manager);

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        Pippip::ContactManager *contactManager;
        Pippip::NicknameManager *nicknameManager;

};

#endif // CONTACTSDIALOG_H
