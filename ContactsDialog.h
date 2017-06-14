#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include "Contact.h"
#include <QDialog>

namespace Ui {
class ContactsDialog;
}

namespace Pippip {
    class ContactManager;
    struct SessionState;
}

class ContactsDialog : public QDialog {
        Q_OBJECT

    public:
        explicit ContactsDialog(Pippip::SessionState *state, QWidget *parent = 0);
        ~ContactsDialog();

    public slots:
        void contactAdded(Pippip::Contact);
        void contactsLoaded();
        void requestsLoaded();

    private slots:
        void addContact();

    public:
        void setManager(Pippip::ContactManager *manager);

    private:
        Ui::ContactsDialog *ui;
        Pippip::SessionState *state;
        Pippip::ContactManager *manager;

};

#endif // CONTACTSDIALOG_H
