/*
 * NicknameHandler.cpp
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

#include "NicknameHandler.h"
#include "ui_NicknamesDialog.h"
#include "Nicknames.h"
#include "AddNicknameTask.h"
#include "LoadNicknamesTask.h"
#include "DeleteNicknameTask.h"
#include "UpdateNicknameTask.h"
#include "Constants.h"
#include "KeyFilter.h"
#include "SessionState.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <assert.h>

static const QString POLICY_NAMES[] = { "Public", "Friends", "Friends of Friends" };

NicknameHandler::NicknameHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  nicknamesValid(false),
  selectedRow(0),
  ui(u) {

    connect(ui->addNicknameButton, SIGNAL(clicked()), this, SLOT(addNickname()));
    connect(ui->deleteNicknameButton, SIGNAL(clicked()), this, SLOT(deleteNickname()));
    connect(ui->nicknameTableWidget, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(editPolicy(int, int)));

    nicknameRE.setPattern("[A-z]+[A-z0-9 _\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new QRegularExpressionValidator(nicknameRE, this);

}

void NicknameHandler::addNickname() {

    newItem = true;
    ui->addNicknameButton->setEnabled(false);
    // Grow the table widget.
    int editRow = ui->nicknameTableWidget->rowCount();
    ui->nicknameTableWidget->setRowCount(editRow + 1);
    loadTable(1);
    // Set up the editing widget.
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->nicknameTableWidget->setCellWidget(0, 0, nicknameLineEdit);

    nicknameLineEdit->setFocus();

}

void NicknameHandler::deleteNickname() {

    int row = ui->nicknameTableWidget->currentRow();
    QLabel *nicknameLabel =
            dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(row, 0));
    QString nickname = nicknameLabel->text();
    Pippip::DeleteNicknameTask *task = new Pippip::DeleteNicknameTask(state, this);
    task->setNickname(nickname);
    connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
    connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
    //task->doRequest();

}

void NicknameHandler::editPolicy(int row, int /* column */) {

    newItem = false;
    ui->addNicknameButton->setEnabled(false);
    ui->deleteNicknameButton->setEnabled(false);
    selectedRow = row;
    //working = (*state->nicknames)[row];
    policyComboBox = new QComboBox;
    QStringList items;
    items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
    policyComboBox->addItems(items);
    KeyFilter *keyFilter = new KeyFilter(policyComboBox);
    keyFilter->addKey(Qt::Key_Enter);
    keyFilter->addKey(Qt::Key_Return);
    keyFilter->addKey(Qt::Key_Tab);
    policyComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(keyPressed(Qt::Key)), this, SLOT(policyEdited(Qt::Key)));
    ui->nicknameTableWidget->setCellWidget(row, 1, policyComboBox);
    //for (int index = 0; index < 3; ++index) {
    //    if ((*state->nicknames)[row].policy == Constants::POLICIES[index]) {
    //        policyComboBox->setCurrentIndex(index);
    //    }
    //}
    policyComboBox->setFocus();

}

const QString& NicknameHandler::getPolicy(const QString& name) const {

    for (int i = 0; i < 3; ++i) {
        if (name == POLICY_NAMES[i]) {
            return Constants::POLICIES[i];
        }
    }
    assert(false);

}

const QString& NicknameHandler::getPolicyName(const QString& policy) const {

    for (int i = 0; i < 3; ++i) {
        if (policy == Constants::POLICIES[i]) {
            return POLICY_NAMES[i];
        }
    }
    assert(false);

}

void NicknameHandler::loadNicknames(Pippip::SessionState *st) {

    state = st;
    Pippip::LoadNicknamesTask *task = new Pippip::LoadNicknamesTask(state, this);
    connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
    connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
            this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
    //task->doRequest();

}

void NicknameHandler::loadTable(int start) {

    ui->nicknameTableWidget->clearContents();
    /*if (state->nicknames->size() > 0) {
        ui->nicknameTableWidget->setRowCount(state->nicknames->size() + start);
        int row = start;
        for (auto nickname : *state->nicknames) {
            QLabel *nameLabel = new QLabel(nickname.entity.nickname);
            nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->nicknameTableWidget->setCellWidget(row, 0, nameLabel);
            QLabel *policyLabel = new QLabel(getPolicyName(nickname.policy));
            policyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->nicknameTableWidget->setCellWidget(row++, 1, policyLabel);
        }

        if (start > 0) {
            ui->nicknameTableWidget->selectRow(0);
        }
        else {
            ui->nicknameTableWidget->selectRow(selectedRow);
        }
    }*/
    ui->addNicknameButton->setEnabled(true);

}

void NicknameHandler::nicknameEdited() {

    working.entity.nickname = nicknameLineEdit->text();
    QLabel *nameLabel = new QLabel(working.entity.nickname);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->nicknameTableWidget->setCellWidget(0, 0, nameLabel);

    policyComboBox = new QComboBox;
    QStringList items;
    items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
    policyComboBox->addItems(items);
    KeyFilter *keyFilter = new KeyFilter(policyComboBox);
    keyFilter->addKey(Qt::Key_Enter);
    keyFilter->addKey(Qt::Key_Return);
    keyFilter->addKey(Qt::Key_Tab);
    policyComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(keyPressed(Qt::Key)), this, SLOT(policyEdited(Qt::Key)));
    ui->nicknameTableWidget->setCellWidget(0, 1, policyComboBox);
    policyComboBox->setFocus();

}

void NicknameHandler::policyEdited(Qt::Key) {

    QString policyName = policyComboBox->currentText();
    QString policy = getPolicy(policyName);

    if (newItem) {
        newItem = false;
        Pippip::AddNicknameTask *task = new Pippip::AddNicknameTask(state, this);
        working.policy = policy;
        task->setNickname(working);
        connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
                this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
        connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
                this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
        //task->doRequest();
    }
    else {
        QLabel *policyLabel = new QLabel(policyName);
        policyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(selectedRow, 1, policyLabel);
        Pippip::UpdateNicknameTask *task = new Pippip::UpdateNicknameTask(state, this);
        task->setUpdateType("policy");
        task->setNickname(working.entity.nickname);
        task->setPolicy(policy);
        connect(task, SIGNAL(requestComplete(Pippip::EnclaveRequestTask*)),
                this, SLOT(requestComplete(Pippip::EnclaveRequestTask*)));
        connect(task, SIGNAL(requestFailed(Pippip::EnclaveRequestTask*)),
                this, SLOT(requestFailed(Pippip::EnclaveRequestTask*)));
        //task->doRequest();
    }
    ui->addNicknameButton->setEnabled(true);
    ui->nicknameTableWidget->setFocus();

}

void NicknameHandler::requestComplete(Pippip::EnclaveRequestTask *task) {
/*
    ui->statusIconLabel->setText(Constants::CHECK_ICON);
    QString taskName = task->getTaskName();
    if (taskName == Constants::ADD_NICKNAME_TASK) {
        ui->statusLabel->setText(working.entity.nickname + " added");
        selectedRow = state->nicknames->size() - 1;
    }
    else if (taskName == Constants::LOAD_NICKNAMES_TASK) {
        ui->statusLabel->setText("Nicknames loaded");
        nicknamesValid = true;
        selectedRow = 0;
    }
    else if (taskName == Constants::DELETE_NICKNAME_TASK) {
        Pippip::DeleteNicknameTask *delTask =
                dynamic_cast<Pippip::DeleteNicknameTask*>(task);
        ui->statusLabel->setText(delTask->getNickname() + " deleted");
        selectedRow = 0;
    }
    else if (taskName == Constants::UPDATE_NICKNAME_TASK) {
        Pippip::UpdateNicknameTask *delTask =
                dynamic_cast<Pippip::UpdateNicknameTask*>(task);
        ui->statusLabel->setText(delTask->getNickname() + " updated");
    }

    loadTable();
    qApp->processEvents();
    task->deleteLater();
*/
}

void NicknameHandler::requestFailed(const QString& error) {

    ui->statusIconLabel->setText(Constants::REDX_ICON);
    if (taskName == Constants::ADD_NICKNAME_TASK) {
        ui->statusLabel->setText(QString("Failed to add nickname - ") + error);
        selectedRow = 0;
    }
    else if (taskName == Constants::LOAD_NICKNAMES_TASK) {
        ui->statusLabel->setText("Failed to load nicknames");
        selectedRow = 0;
    }
    else if (taskName == Constants::DELETE_NICKNAME_TASK) {
        ui->statusLabel->setText(QString("Failed to delete nickname - ") + error);
        selectedRow = 0;
    }
    else if (taskName == Constants::UPDATE_NICKNAME_TASK) {
        ui->statusLabel->setText(QString("Failed to update contact policy - " + error));
        loadTable();
    }

    qApp->processEvents();

}
