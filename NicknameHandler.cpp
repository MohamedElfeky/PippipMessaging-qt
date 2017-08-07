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
#include "NicknameManager.h"
#include "Nicknames.h"
#include "EnclaveResponse.h"
#include "Constants.h"
#include "KeyFilter.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <assert.h>

static const QString POLICY_NAMES[] = { "Public", "Friends", "Friends of Friends" };

NicknameHandler::NicknameHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  firstLoad(true),
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
    // Add a provisional nickname.
    Pippip::Nickname nickname;
    nicknames->append(nickname);
    // This is the only place in NicknameHandler that setCurrentIndex should be called.
    nicknames->setCurrentIndex(editRow);
    // Set up the editing widget.
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->nicknameTableWidget->setCellWidget(editRow, 0, nicknameLineEdit);

    nicknameLineEdit->setFocus();

}

void NicknameHandler::deleteNickname() {

    unsigned currentIndex = nicknames->getCurrentIndex();
    QLabel *nicknameLabel =
            dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(currentIndex, 0));
    working = nicknameLabel->text();
    ui->nicknameTableWidget->removeRow(currentIndex);
    nicknameManager->deleteNickname(working);

}

void NicknameHandler::editPolicy(int row, int /* column */) {

    newItem = false;
    assert(static_cast<unsigned>(row) == nicknames->getCurrentIndex());
    QLabel *nicknameLabel = dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(row, 0));
    if (nicknameLabel != 0) {
        working = nicknameLabel->text();
    }
    ui->addNicknameButton->setEnabled(false);
    ui->deleteNicknameButton->setEnabled(false);
    undo = nicknames->current();
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
    for (int index = 0; index < 3; ++index) {
        if (nicknames->current().policy == Constants::POLICIES[index]) {
            policyComboBox->setCurrentIndex(index);
        }
    }
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

/*
 * This synchronizes the table widget to the nicknames list.
 */
void NicknameHandler::loadTable() {

    ui->nicknameTableWidget->clearContents();
    ui->nicknameTableWidget->setRowCount(nicknames->size());
    int row = 0;
    for (auto nickname : *nicknames) {
        QLabel *nameLabel = new QLabel(nickname.entity.nickname);
        nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row, 0, nameLabel);
        QLabel *policyLabel = new QLabel(getPolicyName(nickname.policy));
        policyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row++, 1, policyLabel);
    }
    ui->nicknameTableWidget->selectRow(nicknames->getCurrentIndex());
    ui->addNicknameButton->setEnabled(true);

}

void NicknameHandler::nicknameAdded(const QString& status) {

    if (status == "added") {
        ui->statusLabel->setText(Constants::CHECK_ICON + working + " added");
    }
    else if (status == "exists") {
        ui->statusLabel->setText(Constants::REDBAR_ICON + working + " exists");
        nicknames->remove(working); // Remove the provisional nickname.
    }
    else {
        ui->statusLabel->setText(Constants::CAUTION_ICON
                                 + "Failed to add nickname - invalid server response");
        nicknames->remove(working); // Remove the provisional nickname.
    }
    loadTable();
    qApp->processEvents();

}

void NicknameHandler::nicknameDeleted(const QString& status) {

    if (status == "deleted") {
        ui->statusLabel->setText(Constants::CHECK_ICON + working + " deleted");
        nicknames->remove(working);
    }
    else if (status == "not found") {   // This should probably never happen.
        ui->statusLabel->setText(Constants::CAUTION_ICON + working + " not found");
    }
    else {
        ui->statusLabel->setText(Constants::CAUTION_ICON
                                 + "Failed to delete nickname - invalid server response");
    }
    qApp->processEvents();

}

void NicknameHandler::nicknameEdited() {

    working = nicknameLineEdit->text();
    nicknames->current().entity.nickname = working;
    QLabel *nameLabel = new QLabel(working);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    unsigned currentIndex = nicknames->getCurrentIndex();
    ui->nicknameTableWidget->setCellWidget(currentIndex, 0, nameLabel);

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
    ui->nicknameTableWidget->setCellWidget(currentIndex, 1, policyComboBox);
    for (int index = 0; index < 3; ++index) {
        if (nicknames->current().policy == Constants::POLICIES[index]) {
            policyComboBox->setCurrentIndex(index);
        }
    }
    policyComboBox->setFocus();

}
/*
void NicknameHandler::nicknameNotFound() {

    ui->statusLabel->setText(working + " not found");
    qApp->processEvents();

}
*/
void NicknameHandler::nicknamesLoaded() {

    loadTable();
    if (firstLoad) {
        ui->statusLabel->setText(Constants::CHECK_ICON + "Nicknames loaded");
        qApp->processEvents();
        firstLoad = false;
    }

}

void NicknameHandler::policyEdited(Qt::Key) {

    QString policyName = policyComboBox->currentText();
    QString policy = getPolicy(policyName);
    nicknames->current().policy = policy;

    if (newItem) {
        // TODO Check for empty whitelist.
        nicknameManager->addNickname(nicknames->current());
        newItem = false;
    }
    else {
        QLabel *policyLabel = new QLabel(policyName);
        policyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        unsigned currentIndex = nicknames->getCurrentIndex();
        ui->nicknameTableWidget->setCellWidget(currentIndex, 1, policyLabel);
        nicknameManager->updatePolicy(working, policy);
    }
    ui->addNicknameButton->setEnabled(true);
    ui->nicknameTableWidget->setFocus();

}

void NicknameHandler::policyUpdated(const QString& status) {

    if (status == "updated") {
        ui->statusLabel->setText(Constants::CHECK_ICON + working + " contact policy updated");
    }
    else if (status == "not found") {
        ui->statusLabel->setText(Constants::REDX_ICON
                                 + "Policy update failed  - Nickname not found");
        nicknames->current() = undo;
    }
    else if (status == "mismatch") {
        ui->statusLabel->setText(Constants::REDX_ICON
                                 + "Policy update failed - Nickname to ID mismatch");
        nicknames->current() = undo;
    }
    else {
        ui->statusLabel->setText(Constants::REDX_ICON + "Policy update failed - Invalid server response");
        nicknames->current() = undo;
    }
    loadTable();
    qApp->processEvents();

}

void NicknameHandler::setManager(Pippip::NicknameManager *manager) {

    nicknameManager = manager;
    nicknames = nicknameManager->getNicknames();

}

void NicknameHandler::taskFailed(const QString& taskName) {

    if (taskName == Constants::ADD_NICKNAME) {
        nicknames->remove(working);
    }
    if (taskName == Constants::UPDATE_CONTACT_POLICY) {
        nicknames->current() = undo;
    }
    loadTable();

}
/*
void NicknameHandler::whitelistUpdated(const Pippip::EntityList &whitelist) {

    working.whitelist = whitelist;

}
*/
