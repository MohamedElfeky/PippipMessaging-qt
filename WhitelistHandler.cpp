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

WhitelistHandler::WhitelistHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  ui(u) {

    connect(ui->addFriendButton, SIGNAL(clicked()), this, SLOT(addEntry()));

    nicknameRE.setPattern("[A-z]+[A-z0-9_\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new EmptyStringValidator(nicknameRE, this);

    puidRE.setPattern("[1-9a-f]+[0-9a-f]{63,}");
    puidRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    puidValidator = new EmptyStringValidator(puidRE, this);

}

void WhitelistHandler::addEntry() {

    newItem = true;
    ui->addNicknameButton->setEnabled(false);
    int editRow = ui->nicknameTableWidget->rowCount();
    ui->nicknameTableWidget->setRowCount(editRow + 1);
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->whitelistTableWidget->setCellWidget(editRow, 0, nicknameLineEdit);

    Pippip::Entity entity;
    whitelist.push_back(entity);
    nicknameLineEdit->setFocus();

}

/*
 * This synchronizes the table widget to the nicknames list.
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
        ui->nicknameTableWidget->setCellWidget(row++, 1, puidLabel);
    }
    ui->addFriendButton->setEnabled(true);
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
        newItem = false;
    }
    else {
        emit whitelistChanged(whitelist);
    }

}

void WhitelistHandler::puidEdited() {

}

void WhitelistHandler::setWhitelist(const Pippip::EntityList &list) {

    whitelist = list;
    loadTable();

}
