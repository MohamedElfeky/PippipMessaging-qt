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
#include "SessionState.h"
#include "NewAccountCreator.h"
#include "NewAccountHelpDialog.h"
#include "mainwindow.h"
#include <QShowEvent>
#include <QMessageBox>
#include <memory>

NewAccountDialog::NewAccountDialog(Pippip::SessionState *sess, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NewAccountDialog),
  session(sess),
  creator(0) {

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(doHelp()));
    const MainWindow *main = qobject_cast<MainWindow*>(parent);

}

NewAccountDialog::~NewAccountDialog() {

    delete ui;
    delete creator;

}

void NewAccountDialog::accept() {

    std::unique_ptr<Pippip::NewAccountCreator> creator(new Pippip::NewAccountCreator(this, session));
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

    if (proceed) {
        ui->progressBar->setValue(0);
        creator->createNewAccount(accountName, passphrase);
    }

}

void NewAccountDialog::accountComplete() {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("New Account Complete");
    message->setText("Successfully created new account");
    message->setIcon(QMessageBox::Information);
    message->exec();
    QDialog::accept();

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

void NewAccountDialog::incrementProgress(int incr) {

    int value = ui->progressBar->value() + incr;
    ui->progressBar->setValue(value);
    qApp->processEvents();

}

bool NewAccountDialog::passphraseAlert() {

    QMessageBox *message = new QMessageBox;
    message->addButton(QMessageBox::Cancel);
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle("New Account Error");
    message->setText("You are about to create an account with no passphrase. This is not recommended");
    message->setInformativeText("Click OK to proceed, Cancel to go back.");
    message->setIcon(QMessageBox::Warning);
    return message->exec() == QMessageBox::Ok;

}

void NewAccountDialog::showEvent(QShowEvent *event) {

    QDialog::showEvent(event);
    if (!event->spontaneous()) {
        ui->AccountNameText->setFocus();
    }

}

void NewAccountDialog::updateInfo(QString info) {

    ui->infoLabel->setText(info);
    qApp->processEvents();

}

