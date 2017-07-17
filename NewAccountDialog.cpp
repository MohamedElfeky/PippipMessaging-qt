/*
 * NewAccountDialog.cpp
 * Copyright (C) 2017 Steve Brenneis <steve.brenneis@secomm.org>
 *
 * PippipMessaging is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PippipMessaging is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "NewAccountDialog.h"
#include "ui_NewAccountDialog.h"
#include "ParameterGenerator.h"
#include "NewAccountCreator.h"
#include "NewAccountHelpDialog.h"
#include "mainwindow.h"
#include <QShowEvent>
#include <QMessageBox>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

NewAccountDialog::NewAccountDialog(Pippip::ParameterGenerator *gen, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NewAccountDialog),
  generator(gen),
  creator(0) {

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(doHelp()));
    MainWindow *mainWindow = dynamic_cast<MainWindow*>(parent);
    creator = new Pippip::NewAccountCreator(this, generator);
    connect(creator, SIGNAL(accountComplete(int)), mainWindow, SLOT(loggedIn()));
    connect(creator, SIGNAL(accountComplete(QString)), this, SLOT(accountComplete(QString)));

}

NewAccountDialog::~NewAccountDialog() {

    delete ui;

}

void NewAccountDialog::accept() {

    bool proceed = true;
    QString accountName = ui->AccountNameText->text();
    if (accountName.length() == 0) {
        accountNameAlert();
        proceed = false;
    }

    QString passphrase = ui->PassphraseText->text();
    if (proceed && passphrase.length() == 0) {
        proceed = passphraseAlert();
    }

    QSettings settings;
    QByteArray json = settings.value("User/accounts").value<QByteArray>();
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QJsonObject accountsObj = doc.object();
    QJsonArray accounts = accountsObj["users"].toArray();
    for (auto account : accounts) {
        if (account.toString() == accountName) {
            proceed = duplicateAccountAlert();
        }
    }

    if (proceed) {
        ui->progressBar->setValue(0);
        creator->createNewAccount(accountName, passphrase);
    }

}

void NewAccountDialog::accountComplete(const QString& account) {

    ui->AccountNameText->clear();
    ui->PassphraseText->clear();
    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("Account Complete");
    message->setText("New account created for " + account);
    message->setIcon(QMessageBox::Information);
    message->exec();

}

void NewAccountDialog::accountNameAlert() {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("New Account Error");
    message->setText("Account name must not be empty.");
    message->setIcon(QMessageBox::Critical);
    message->exec();

}

void NewAccountDialog::doHelp() {

    NewAccountHelpDialog dialog;
    dialog.exec();

}

bool NewAccountDialog::duplicateAccountAlert() {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Cancel);
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("New Account Warning");
    message->setText("You are about to overwrite an existing account. This will make that account unusable.");
    message->setInformativeText("Click OK to proceed, Cancel to go back.");
    message->setIcon(QMessageBox::Warning);
    return message->exec() == QMessageBox::Ok;

}

void NewAccountDialog::incrementProgress(int incr) {

    int value = ui->progressBar->value() + incr;
    ui->progressBar->setValue(value);
    qApp->processEvents();

}

bool NewAccountDialog::passphraseAlert() {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Cancel);
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("New Account Warning");
    message->setText("You are about to create an account with no passphrase. This is not recommended");
    message->setInformativeText("Click OK to proceed, Cancel to go back.");
    message->setIcon(QMessageBox::Warning);
    return message->exec() == QMessageBox::Ok;

}

void NewAccountDialog::resetProgress() {

    ui->progressBar->setValue(0);
    qApp->processEvents();

}

void NewAccountDialog::showEvent(QShowEvent *event) {

    // Have to do this to give account name the focus.
    QDialog::showEvent(event);
    if (!event->spontaneous()) {
        ui->AccountNameText->setFocus();
    }

}

void NewAccountDialog::updateInfo(QString info) {

    ui->infoLabel->setText(info);
    qApp->processEvents();

}

