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
#include "SessionState.h"
#include "Nicknames.h"
#include "Constants.h"
#include "UpdateNicknameTask.h"
#include <QLineEdit>
#include <QLabel>
#include <QClipboard>
#include <QDebug>
#include <assert.h>

WhitelistHandler::WhitelistHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  selectedRow(0),
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
    ui->addFriendButton->setEnabled(false);
    ui->deleteFriendButton->setEnabled(false);
    ui->pastePushButton->setEnabled(true);
    // Grow the table widget
    //unsigned size = (*state->nicknames)[currentNickname].whitelist.size();
    //ui->whitelistTableWidget->setRowCount(size + 1);
    loadTable(1);
    selectedRow = 0;
    // Set up the nickname editing widget
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    ui->whitelistTableWidget->setCellWidget(0, 0, nicknameLineEdit);
    nicknameLineEdit->setFocus();
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));

}

void WhitelistHandler::deleteEntry() {

    newItem = false;
    selectedRow = ui->whitelistTableWidget->currentRow();

    Pippip::UpdateNicknameTask *task = new Pippip::UpdateNicknameTask(state, this);
    task->setUpdateType("whitelist");
    task->setUpdateAction("delete");
    //task->setWhitelistEntity((*state->nicknames)[currentNickname].whitelist[selectedRow]);
    //task->setNickname((*state->nicknames)[currentNickname].entity.nickname);
    connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
    connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
    //task->doRequest();
}

/*
 * Public ID editing only.
 */
void WhitelistHandler::editEntry(int row, int /* column */) {

    newItem = false;
    selectedRow = row;

    ui->addFriendButton->setEnabled(false);
    ui->deleteFriendButton->setEnabled(false);
    ui->pastePushButton->setEnabled(true);

    //Pippip::Entity& wlEntry = (*state->nicknames)[currentNickname].whitelist[selectedRow];
    //working.nickname = wlEntry.nickname;
    //puidLineEdit = new QLineEdit(wlEntry.publicId);
    puidLineEdit->setValidator(puidValidator);
    ui->whitelistTableWidget->setCellWidget(selectedRow, 1, puidLineEdit);
    puidLineEdit->setFocus();
    connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));

}

void WhitelistHandler::loadTable(int start) {

    ui->whitelistTableWidget->clearContents();
    //const Pippip::Nickname& nickname = (*state->nicknames)[currentNickname];
    /*if (nickname.whitelist.size() > 0) {
        ui->whitelistTableWidget->setRowCount(nickname.whitelist.size() + start);
        int row = start;
        for (auto entity : nickname.whitelist) {
            QLabel *nameLabel = new QLabel(entity.nickname);
            nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->whitelistTableWidget->setCellWidget(row, 0, nameLabel);
            QLabel *puidLabel = new QLabel(entity.publicId);
            puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->whitelistTableWidget->setCellWidget(row++, 1, puidLabel);
        }
        if (start > 0) {
            ui->whitelistTableWidget->selectRow(0);
        }
        else {
            ui->whitelistTableWidget->selectRow(selectedRow);
        }
    }*/

}

void WhitelistHandler::nicknameEdited() {

    QString nickname = nicknameLineEdit->text();
    QLabel *nameLabel = new QLabel(nickname);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setCellWidget(selectedRow, 0, nameLabel);
    working.nickname = nickname;

    puidLineEdit = new QLineEdit;
    puidLineEdit->setValidator(puidValidator);
    ui->whitelistTableWidget->setCellWidget(0, 1, puidLineEdit);
    puidLineEdit->setFocus();
    connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));

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

    // Firing twice. Investigate later.
    disconnect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
    QString publicId = puidLineEdit->text();
    QLabel *puidLabel = new QLabel(publicId);
    puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->whitelistTableWidget->setCellWidget(selectedRow, 1, puidLabel);
    working.publicId = publicId;

    QString action = newItem ? "add" : "modify";
    Pippip::UpdateNicknameTask *task = new Pippip::UpdateNicknameTask(state, this);
    task->setUpdateType("whitelist");
    task->setUpdateAction(action);
    task->setWhitelistEntity(working);
    //task->setNickname((*state->nicknames)[currentNickname].entity.nickname);
    connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
    connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
    //task->doRequest();

    ui->addFriendButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);

}

void WhitelistHandler::requestComplete(Pippip::EnclaveRequestTask *task) {

    ui->statusIconLabel->setText(Constants::CHECK_ICON);
    Pippip::UpdateNicknameTask *updateTask =
            dynamic_cast<Pippip::UpdateNicknameTask*>(task);
    QString action = updateTask->getUpdateAction();
    if (action == "add") {
        ui->statusLabel->setText("Friend added");
        //selectedRow = (*state->nicknames)[currentNickname].whitelist.size() - 1;
    }
    else if (action == "delete") {
        ui->statusLabel->setText("Friend deleted");
        selectedRow = 0;
    }
    else if (action == "modify") {
        ui->statusLabel->setText("Friend updated");
    }

    loadTable();
    qApp->processEvents();
    task->deleteLater();

}

void WhitelistHandler::requestFailed(Pippip::EnclaveRequestTask *task) {
/*
    selectedRow = 0;
    ui->statusIconLabel->setText(Constants::REDX_ICON);
    Pippip::UpdateNicknameTask *updateTask =
            dynamic_cast<Pippip::UpdateNicknameTask*>(task);
    QString action = updateTask->getUpdateAction();
    if (action == "add") {
        ui->statusLabel->setText("Unable to add friend - " + task->getError());
    }
    else if (action == "delete") {
        ui->statusLabel->setText("Unable to delete friend - " + task->getError());
    }
    else if (action == "modify") {
        ui->statusLabel->setText("Unable to update friend - " + task->getError());
    }

    loadTable();
    qApp->processEvents();
    task->deleteLater();
*/
}
