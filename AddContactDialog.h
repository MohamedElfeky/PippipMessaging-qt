#ifndef ADDCONTACTDIALOG_H
#define ADDCONTACTDIALOG_H

#include <QDialog>

namespace Ui {
    class AddContactDialog;
}

class AddContactDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit AddContactDialog(QWidget *parent = 0);
        ~AddContactDialog();

    public:
        void accept();
        const QString& getId() const { return id; }
        const QString& getIdType() const { return idType; }

    private:
        Ui::AddContactDialog *ui;
        QString id;
        QString idType;

};

#endif // ADDCONTACTDIALOG_H
