#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

namespace Pippip {
    class Vault;
    class Authenticator;
}

class LoginDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit LoginDialog(Pippip::Vault *vault, QWidget *parent = 0);
        ~LoginDialog();

    public slots:
        void login();
        void loginReset();
        void updateInfo(QString);

    private slots:
        void sessionFailed(const QString& error);

    protected:
        void showEvent(QShowEvent *);

/*    private:
        void accountNameAlert();
*/
    private:
        Ui::LoginDialog *ui;
        Pippip::Vault *vault;
        Pippip::Authenticator *auth;

};

#endif // LOGINDIALOG_H
