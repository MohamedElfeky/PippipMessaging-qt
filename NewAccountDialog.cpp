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
#include "NewAccountCreator.h"
#include "NewAccountHelpDialog.h"

NewAccountDialog::NewAccountDialog(QWidget *parent)
: QDialog(parent),
  ui(new Ui::NewAccountDialog),
  creator(0) {

    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(helpRequested()), this, SLOT(doHelp()));

}

NewAccountDialog::~NewAccountDialog() {

    delete ui;
    delete creator;

}

void NewAccountDialog::accept() {

    creator = new Pippip::NewAccountCreator;
    QString accountName = ui->AccountNameText->text();
    QString passphrase = ui->PassphraseText->text();
    creator->createNewAccount(accountName.toUtf8().constData(), passphrase.toUtf8().constData());

}

void NewAccountDialog::doHelp() {

    NewAccountHelpDialog dialog;
    dialog.exec();

}
