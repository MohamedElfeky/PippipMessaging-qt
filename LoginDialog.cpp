#include "LoginDialog.h"
#include "Authenticator.h"
#include "ui_LoginDialog.h"
#include <QPushButton>
#include <QMessageBox>
#include <QShowEvent>

LoginDialog::LoginDialog(Pippip::Vault *v, QWidget *parent)
: QDialog(parent),
  ui(new Ui::LoginDialog),
  vault(v),
  auth(0) {

    ui->setupUi(this);
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(login()));

}

LoginDialog::~LoginDialog() {

    delete ui;

}

void LoginDialog::accountNameAlert() {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("Login Error");
    message->setText("Account name must not be empty.");
    message->setIcon(QMessageBox::Critical);
    message->exec();

}

void LoginDialog::login() {

    QString accountName = ui->AccountNameText->text();
    QString passphrase = ui->PassphraseText->text();
    if (accountName.length() == 0) {
        accountNameAlert();
    }
    else {
        auth = new Pippip::Authenticator(this, vault);
        auth->authenticate(accountName, passphrase);
    }

}

void LoginDialog::showEvent(QShowEvent *event) {

    // Have to do this to give account name the focus.
    QDialog::showEvent(event);
    if (!event->spontaneous()) {
        ui->AccountNameText->setFocus();
    }

}

void LoginDialog::updateInfo(QString info) {

    ui->infoLabel->setText(info);
    qApp->processEvents();

}


