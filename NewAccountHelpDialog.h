#ifndef NEWACCOUNTHELPDIALOG_H
#define NEWACCOUNTHELPDIALOG_H

#include <QDialog>

namespace Ui {
class NewAccountHelpDialog;
}

class NewAccountHelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewAccountHelpDialog(QWidget *parent = 0);
    ~NewAccountHelpDialog();

private:
    Ui::NewAccountHelpDialog *ui;
};

#endif // NEWACCOUNTHELPDIALOG_H
