#include "LoginDialog.h"
#include "Authenticator.h"
#include "mainwindow.h"
#include "Constants.h"
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
    connect(auth, SIGNAL(authenticationComplete(bool)), main, SLOT(loggedIn(bool)));
    connect(auth, SIGNAL(loginReset()), this, SLOT(loginReset()));
    connect(auth, SIGNAL(sessionFailed(QString)), this, SLOT(sessionFailed(QString)));

}

LoginDialog::~LoginDialog() {

    delete ui;

}
/*
void LoginDialog::accountNameAlert() {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("Login Error");
    message->setText("Account name must not be empty.");
    message->setIcon(QMessageBox::Critical);
    message->exec();

}
*/
void LoginDialog::login() {

    QString accountName = ui->accountNameComboBox->currentText();
    QString passphrase = ui->PassphraseText->text();
    ui->loginButton->setEnabled(false);
    //if (accountName.length() == 0) {
    //    accountNameAlert();
    //}
    //else {
        auth->reset();
        auth->authenticate(accountName, passphrase);
    //}

}

void LoginDialog::loginReset() {

    ui->loginButton->setEnabled(true);
    ui->PassphraseText->clear();
    //ui->accountNameComboBox->setCurrentIndex(0);
    qApp->processEvents();

}

void LoginDialog::sessionFailed(const QString &error) {

    ui->PassphraseText->clear();
    ui->loginButton->setEnabled(true);
    ui->infoLabel->setText(Constants::REDX_ICON + "Failed to establish session - "
                           + error);
    qApp->processEvents();

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

