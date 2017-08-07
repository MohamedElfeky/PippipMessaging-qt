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
#include "NicknameManager.h"
#include "Nicknames.h"
#include "Constants.h"
#include <QLineEdit>
#include <QLabel>
#include <QClipboard>
#include <assert.h>

WhitelistHandler::WhitelistHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  currentIndex(0),
  ui(u) {

    connect(ui->addFriendButton, SIGNAL(clicked()), this, SLOT(addEntry()));
    connect(ui->deleteFriendButton, SIGNAL(clicked()), this, SLOT(deleteEntry()));
    connect(ui->whitelistTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(editEntry(int,int)));
    connect(ui->pastePushButton, SIGNAL(clicked()), this, SLOT(pasteValue()));

    nicknameRE.setPattern("[A-z]+[A-z0-9 _\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new EmptyStringValidator(nicknameRE, this);

    puidRE.setPattern("[1-9a-fA-F]+[0-9a-fA-F]{19,}");
    puidRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    puidValidator = new EmptyStringValidator(puidRE, this);

}

void WhitelistHandler::addEntry() {

    newItem = true;
    undo = nicknames->current().whitelist;
    ui->addFriendButton->setEnabled(false);
    ui->deleteFriendButton->setEnabled(false);
    ui->pastePushButton->setEnabled(true);
    // Grow the table widget
    // This is the only place this should be set directly.
    currentIndex = ui->whitelistTableWidget->rowCount();
    ui->whitelistTableWidget->setRowCount(currentIndex + 1);
    // Add a provisional entity
    Pippip::Entity entity;
    nicknames->current().whitelist.push_back(entity);
    // Set up the nickname editing widget
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    ui->whitelistTableWidget->setCellWidget(currentIndex, 0, nicknameLineEdit);
    nicknameLineEdit->setFocus();
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));

}

void WhitelistHandler::deleteEntry() {

    newItem = false;
    undo = nicknames->current().whitelist;
    assert(static_cast<int>(currentIndex) == ui->whitelistTableWidget->currentRow());
    QLabel *nicknameLabel =
            dynamic_cast<QLabel*>(ui->whitelistTableWidget->cellWidget(currentIndex,0));
    QString wlNickname = nicknameLabel->text();
    QLabel *puidLabel =
            dynamic_cast<QLabel*>(ui->whitelistTableWidget->cellWidget(currentIndex,1));
    QString wlPublicId = puidLabel->text();
    assert(nicknames->current().whitelist[currentIndex].nickname == wlNickname
            && nicknames->current().whitelist[currentIndex].publicId == wlPublicId);
    Pippip::Entity wlEntity = nicknames->current().whitelist[currentIndex];
    removeEntry(wlEntity);
    QString workingNickname = nicknames->current().entity.nickname;
    nicknameManager->updateWhitelist(workingNickname, wlEntity, "delete");

}

/*
 * Public ID editing only.
 */
void WhitelistHandler::editEntry(int row, int column) {

    newItem = false;
    undo = nicknames->current().whitelist;
    assert(static_cast<int>(currentIndex) == row);
    ui->addFriendButton->setEnabled(false);
    ui->deleteFriendButton->setEnabled(false);
    ui->pastePushButton->setEnabled(true);

    switch (column) {
        case 0:
            nicknameLineEdit = new QLineEdit;
            nicknameLineEdit->setValidator(nicknameValidator);
            ui->whitelistTableWidget->setCellWidget(currentIndex, 0, nicknameLineEdit);
            nicknameLineEdit->setFocus();
            connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
            break;
        case 1:
            puidLineEdit = new QLineEdit(nicknames->current().whitelist[currentIndex].publicId);
            puidLineEdit->setValidator(puidValidator);
            ui->whitelistTableWidget->setCellWidget(currentIndex, 1, puidLineEdit);
            puidLineEdit->setFocus();
            connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
            break;
    }


}

/*
 * This synchronizes the table widget to the whitelist.
 */
void WhitelistHandler::loadTable() {

    ui->whitelistTableWidget->clearContents();
    ui->whitelistTableWidget->setRowCount(nicknames->current().whitelist.size());
    int row = 0;
    for (auto entity : nicknames->current().whitelist) {
        QLabel *nameLabel = new QLabel(entity.nickname);
        nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->whitelistTableWidget->setCellWidget(row, 0, nameLabel);
        QLabel *puidLabel = new QLabel(entity.publicId);
        puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->whitelistTableWidget->setCellWidget(row++, 1, puidLabel);
    }
    ui->addFriendButton->setEnabled(true);
    if (nicknames->current().whitelist.size() > 0) {
        ui->whitelistTableWidget->selectRow(0);
    }
    else {
        ui->deleteFriendButton->setEnabled(false);
    }

}

void WhitelistHandler::nicknameEdited() {

    QString nickname = nicknameLineEdit->text();
    QLabel *nameLabel = new QLabel(nickname);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setCellWidget(currentIndex, 0, nameLabel);
    nicknames->current().whitelist[currentIndex].nickname = nickname;

    if (newItem) {
        puidLineEdit = new QLineEdit;
        puidLineEdit->setValidator(puidValidator);
        ui->whitelistTableWidget->setCellWidget(currentIndex, 1, puidLineEdit);
        puidLineEdit->setFocus();
        connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
    }
    else {
        QString action = "modify";
        QString workingNickname = nicknames->current().entity.nickname;
        Pippip::Entity wlEntity = nicknames->current().whitelist[currentIndex];
        nicknameManager->updateWhitelist(workingNickname, wlEntity, action);
        ui->addFriendButton->setEnabled(true);
        ui->pastePushButton->setEnabled(false);
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

    QString publicId = puidLineEdit->text();
    QLabel *puidLabel = new QLabel(publicId);
    puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setCellWidget(currentIndex, 1, puidLabel);
    nicknames->current().whitelist[currentIndex].publicId = publicId;

    QString action = newItem ? "add" : "modify";
    QString workingNickname = nicknames->current().entity.nickname;
    Pippip::Entity wlEntity = nicknames->current().whitelist[currentIndex];
    nicknameManager->updateWhitelist(workingNickname, wlEntity, action);
    ui->addFriendButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);

}

void WhitelistHandler::removeEntry(const Pippip::Entity& entity) {

    Pippip::EntityList temp;

    Pippip::EntityList& working = nicknames->current().whitelist;
    for (auto entry : working) {
        if (entry.nickname != entity.nickname || entry.publicId != entity.publicId) {
            temp.push_back(entry);
        }
    }
    nicknames->current().whitelist.swap(temp);

}
/*
void WhitelistHandler::setNickname(const Pippip::Nickname &nickname) {

    workingNickname = nickname.entity.nickname;
    workingWhitelist = nickname.whitelist;
    currentIndex = 0;
    loadTable();

}
*/

void WhitelistHandler::setCurrentIndex(unsigned index) {

    assert(index < nicknames->current().whitelist.size());
    currentIndex = index;

}

void WhitelistHandler::setNicknameManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    nicknames = nicknameManager->getNicknames();

}

void WhitelistHandler::taskFailed(const QString& /* taskName */) {

    nicknames->current().whitelist = undo;
    currentIndex = 0;
    loadTable();

}

void WhitelistHandler::whitelistUpdated(const QString& status) {

    if (status == "updated") {
        ui->statusLabel->setText(Constants::CHECK_ICON
                                 + nicknames->current().entity.nickname + " friends list updated");
    }
    else if (status == "not found") {
        ui->statusLabel->setText(Constants::REDX_ICON
                                 + "Friends list update failed  - Nickname not found");
        nicknames->current().whitelist = undo;
    }
    else if (status == "whitelist not found") {     // Probably should never happen
        ui->statusLabel->setText(Constants::REDX_ICON
                                 + "Friends list update failed  - Friend not found");
        nicknames->current().whitelist = undo;
    }
    else if (status == "mismatch") {
        ui->statusLabel->setText(Constants::REDX_ICON
                                 + "Friends list update failed - Nickname to ID mismatch");
        nicknames->current().whitelist = undo;
    }
    else {
        ui->statusLabel->setText(Constants::REDX_ICON
                                 + "Friends list update failed - Invalid server response");
        nicknames->current().whitelist = undo;
    }
    loadTable();
    qApp->processEvents();

}
