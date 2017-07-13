#include "LoginDialog.h"
#include "Authenticator.h"
#include "mainwindow.h"
#include "ui_LoginDialog.h"
#include <QPushButton>
#include <QMessageBox>
#include <QShowEvent>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

LoginDialog::LoginDialog(Pippip::Vault *v, QWidget *parent)
: QDialog(parent),
  ui(new Ui::LoginDialog),
  vault(v),
  auth(0) {

    ui->setupUi(this);
    QSettings settings;
    QByteArray json = settings.value("User/accounts").value<QByteArray>();
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject user = doc.object();
    QJsonArray users = user["users"].toArray();
    QStringList items;
    for (auto username : users) {
        items << username.toString();
    }
    ui->accountNameComboBox->addItems(items);

    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(login()));
    MainWindow *main = dynamic_cast<MainWindow*>(parent);
    auth = new Pippip::Authenticator(this, vault);
    connect(auth, SIGNAL(authenticationComplete(QString)), main, SLOT(loggedIn(QString)));

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

    QString accountName = ui->accountNameComboBox->currentText();
    QString passphrase = ui->PassphraseText->text();
    if (accountName.length() == 0) {
        accountNameAlert();
    }
    else {
        auth->reset();
        auth->authenticate(accountName, passphrase);
    }

}

void LoginDialog::showEvent(QShowEvent *event) {

    // Have to do this to give account name the focus.
    QDialog::showEvent(event);
    if (!event->spontaneous()) {
        ui->accountNameComboBox->setFocus();
    }

}

void LoginDialog::updateInfo(QString info) {

    ui->infoLabel->setText(info);
    qApp->processEvents();

}


