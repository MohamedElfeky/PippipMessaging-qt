/*
 * WhitelistHandler.cpp
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

#include "WhitelistHandler.h"
#include "ui_NicknamesDialog.h"
#include "EmptyStringValidator.h"
#include <QLineEdit>
#include <QLabel>
#include <QClipboard>
#include <assert.h>

WhitelistHandler::WhitelistHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  ui(u) {

    connect(ui->addFriendButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->deleteFriendButton, SIGNAL(clicked()), this, SLOT(deleteEntry()));
    connect(ui->whitelistTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(editEntry(int,int)));
    connect(ui->pastePushButton, SIGNAL(clicked()), this, SLOT(pasteValue()));

    nicknameRE.setPattern("[A-z]+[A-z0-9_\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new EmptyStringValidator(nicknameRE, this);

    puidRE.setPattern("[1-9a-f]+[0-9a-f]{19,}");
    puidRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    puidValidator = new EmptyStringValidator(puidRE, this);

}

void WhitelistHandler::addEntry() {

    newItem = true;
    ui->addFriendButton->setEnabled(false);
    ui->deleteFriendButton->setEnabled(false);
    ui->pastePushButton->setEnabled(true);
    int editRow = ui->whitelistTableWidget->rowCount();
    ui->whitelistTableWidget->setRowCount(editRow + 1);
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->whitelistTableWidget->setCellWidget(editRow, 0, nicknameLineEdit);
    nicknameLineEdit->setFocus();

    Pippip::Entity entity;
    whitelist.push_back(entity);
    nicknameLineEdit->setFocus();

}

void WhitelistHandler::deleteEntry() {

    int row = ui->whitelistTableWidget->currentRow();
    removeEntry(row);
    emit whitelistChanged(whitelist);

}

void WhitelistHandler::editEntry(int row, int column) {

    newItem = false;
    ui->addFriendButton->setEnabled(false);
    ui->deleteFriendButton->setEnabled(false);
    if (column == 0) {
        nicknameLineEdit = new QLineEdit(whitelist[row].nickname);
        nicknameLineEdit->setValidator(nicknameValidator);
        connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
        ui->whitelistTableWidget->setCellWidget(row, 0, nicknameLineEdit);
        nicknameLineEdit->setFocus();
    }
    else if (column == 1){
        puidLineEdit = new QLineEdit(whitelist[row].publicId);
        puidLineEdit->setValidator(puidValidator);
        connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
        ui->whitelistTableWidget->setCellWidget(row, 1, puidLineEdit);
        puidLineEdit->setFocus();
    }
    else {
        assert(false);
    }

}

/*
 * This synchronizes the table widget to the whitelist.
 */
void WhitelistHandler::loadTable() {

    ui->whitelistTableWidget->clearContents();
    ui->whitelistTableWidget->setRowCount(whitelist.size());
    int row = 0;
    for (auto entity : whitelist) {
        QLabel *nameLabel = new QLabel(entity.nickname);
        nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->whitelistTableWidget->setCellWidget(row, 0, nameLabel);
        QLabel *puidLabel = new QLabel(entity.publicId);
        puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->whitelistTableWidget->setCellWidget(row++, 1, puidLabel);
    }
    ui->addFriendButton->setEnabled(true);
    if (whitelist.size() > 0) {
        ui->whitelistTableWidget->selectRow(0);
    }
    else {
        ui->deleteFriendButton->setEnabled(false);
    }

}

void WhitelistHandler::nicknameEdited() {

    int row = ui->whitelistTableWidget->currentRow();
    QString nickname = nicknameLineEdit->text();
    QLabel *nameLabel = new QLabel(nickname);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setCellWidget(row, 0, nameLabel);
    whitelist[row].nickname = nickname;

    if (newItem) {
        puidLineEdit = new QLineEdit;
        puidLineEdit->setValidator(puidValidator);
        connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
        ui->whitelistTableWidget->setCellWidget(row, 1, puidLineEdit);
        puidLineEdit->setFocus();
    }
    else if (whitelist[row].nickname.length() > 0 || whitelist[row].publicId.length() > 0) {
        ui->addFriendButton->setEnabled(true);
        ui->pastePushButton->setEnabled(false);
        emit whitelistChanged(whitelist);
    }
    else {
        ui->addFriendButton->setEnabled(true);
        ui->pastePushButton->setEnabled(false);
        removeEntry(row);
    }

}

void WhitelistHandler::pasteValue() {

    QClipboard *clipboard = QApplication::clipboard();
    int column = ui->whitelistTableWidget->currentColumn();
    if (column == 0) {
        nicknameLineEdit->setText(clipboard->text());
    }
    else if (column == 1) {
        puidLineEdit->setText(clipboard->text());
    }
    else {
        assert(false);
    }

}

void WhitelistHandler::puidEdited() {

    int row = ui->whitelistTableWidget->currentRow();
    QString publicId = puidLineEdit->text();
    QLabel *puidLabel = new QLabel(publicId);
    puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setCellWidget(row, 1, puidLabel);
    whitelist[row].publicId = publicId;

    ui->addFriendButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);
    if (whitelist[row].nickname.length() == 0 && whitelist[row].publicId.length() == 0) {
        removeEntry(row);
    }
    else {
        emit whitelistChanged(whitelist);
    }

}

void WhitelistHandler::removeEntry(unsigned index) {

    Pippip::EntityList temp;

    for (unsigned i = 0; i < whitelist.size(); ++i) {
        if (i != index) {
            temp.push_back(whitelist[i]);
        }
    }
    whitelist.swap(temp);
    loadTable();

}

void WhitelistHandler::setWhitelist(const Pippip::EntityList &list) {

    whitelist = list;
    loadTable();

}
