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
#include "EnterKeyFilter.h"
#include "NicknameManager.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <assert.h>

static const QString POLICIES[] = { "Public", "FriendsOnly", "FriendsOfFriends" };
static const QString POLICY_NAMES[] = { "Public", "Friends", "Friends of Friends" };

NicknameHandler::NicknameHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  ui(u) {

    connect(ui->addNicknameButton, SIGNAL(clicked()), this, SLOT(addNickname()));
    connect(ui->deleteNicknameButton, SIGNAL(clicked()), this, SLOT(deleteNickname()));
    connect(ui->nicknameTableWidget, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(editPolicy(int, int)));

    nicknameRE.setPattern("[A-z]+[A-z0-9_\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new QRegularExpressionValidator(nicknameRE, this);

}

void NicknameHandler::addNickname() {

    newItem = true;
    ui->addNicknameButton->setEnabled(false);
    int editRow = ui->nicknameTableWidget->rowCount();
    ui->nicknameTableWidget->setRowCount(editRow + 1);
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->nicknameTableWidget->setCellWidget(editRow, 0, nicknameLineEdit);

    Pippip::Nickname newNick;
    nicknames.push_back(newNick);
    nicknameLineEdit->setFocus();

}

const Pippip::Nickname& NicknameHandler::currentNickname() const {

    unsigned row = ui->nicknameTableWidget->currentRow();
    assert(row < nicknames.size());
    return nicknames[row];

}

void NicknameHandler::deleteNickname() {

    int row = ui->nicknameTableWidget->currentRow();
    QLabel *nicknameLabel = dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(row, 0));
    manager->deleteNickname(nicknameLabel->text());

}

void NicknameHandler::editPolicy(int row, int /* column */) {

    newItem = false;
    ui->addNicknameButton->setEnabled(false);
    ui->deleteNicknameButton->setEnabled(false);
    policyComboBox = new QComboBox;
    QStringList items;
    items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
    policyComboBox->addItems(items);
    EnterKeyFilter *keyFilter = new EnterKeyFilter(policyComboBox);
    policyComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(enterPressed()), this, SLOT(policySelected()));
    ui->nicknameTableWidget->setCellWidget(row, 1, policyComboBox);
    for (int index = 0; index < 3; ++index) {
        if (nicknames[row].policy == POLICIES[index]) {
            policyComboBox->setCurrentIndex(index);
        }
    }
    // Do this here so the event doesn't fire when the value is changed above.
    connect(policyComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(policyChanged(QString)));
    policyComboBox->setFocus();

}

const QString& NicknameHandler::getPolicy(const QString& name) const {

    for (int i = 0; i < 3; ++i) {
        if (name == POLICY_NAMES[i]) {
            return POLICIES[i];
        }
    }
    assert(false);

}

const QString& NicknameHandler::getPolicyName(const QString& policy) const {

    for (int i = 0; i < 3; ++i) {
        if (policy == POLICIES[i]) {
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
    ui->nicknameTableWidget->setRowCount(nicknames.size());
    int row = 0;
    for (auto nickname : nicknames) {
        QLabel *nameLabel = new QLabel(nickname.entity.nickname);
        nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row, 0, nameLabel);
        QLabel *policyLabel = new QLabel(getPolicyName(nickname.policy));
        policyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row++, 1, policyLabel);
    }
    ui->addNicknameButton->setEnabled(true);

}

void NicknameHandler::nicknameAdded(const QString& name) {

    nicknames = manager->getNicknames();
    loadTable();
    for (size_t row = 0; row < nicknames.size(); ++row) {
        if (nicknames[row].entity.nickname == name) {
            ui->nicknameTableWidget->selectRow(row);
        }
    }
    ui->statusLabel->setText(name + " added");
    qApp->processEvents();

}

void NicknameHandler::nicknameDeleted(const QString& name) {

    nicknames = manager->getNicknames();
    loadTable();
    ui->statusLabel->setText(name + " deleted");
    qApp->processEvents();

}

void NicknameHandler::nicknameEdited() {

    int row = ui->nicknameTableWidget->currentRow();
    QString nickname = nicknameLineEdit->text();
    nicknames[row].entity.nickname = nickname;
    QLabel *nameLabel = new QLabel(nickname);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->nicknameTableWidget->setCellWidget(row, 0, nameLabel);

    policyComboBox = new QComboBox;
    QStringList items;
    items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
    policyComboBox->addItems(items);
    EnterKeyFilter *keyFilter = new EnterKeyFilter(policyComboBox);
    policyComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(enterPressed()), this, SLOT(policySelected()));
    ui->nicknameTableWidget->setCellWidget(row, 1, policyComboBox);
    for (int index = 0; index < 3; ++index) {
        if (nicknames[row].policy == POLICIES[index]) {
            policyComboBox->setCurrentIndex(index);
        }
    }
    // Do this here so the event doesn't fire when the value is changed above.
    connect(policyComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(policyChanged(QString)));
    policyComboBox->setFocus();

}

void NicknameHandler::nicknamesLoaded() {

    nicknames = manager->getNicknames();
    loadTable();
    if (nicknames.size() > 0) {
        ui->nicknameTableWidget->selectRow(0);
    }
    ui->statusLabel->setText("Nicknames loaded");
    qApp->processEvents();

}

void NicknameHandler::nicknameUpdated(Pippip::Nickname nickname) {

    int row = ui->nicknameTableWidget->currentRow();
    nicknames[row].policy = nickname.policy;
    QLabel *policyLabel = dynamic_cast<QLabel*>(ui->nicknameTableWidget->cellWidget(row, 1));
    policyLabel->setText(getPolicyName(nickname.policy));
    ui->addNicknameButton->setEnabled(true);
    ui->deleteNicknameButton->setEnabled(true);
    ui->statusLabel->setText(nickname.entity.nickname + " updated");
    qApp->processEvents();
    emit policyUpdated(nickname.policy);

}

/*
 * Signaled when policy combo box selection changes.
 */
void NicknameHandler::policyChanged(const QString& value) {

    QString policy = getPolicy(value);
    policyEdited(policy);

}

void NicknameHandler::policyEdited(const QString &policy) {

    unsigned row = ui->nicknameTableWidget->currentRow();
    assert(row < nicknames.size());

    nicknames[row].policy = policy;
    //ui->nicknameTableWidget->removeCellWidget(row, 1);

    if (newItem) {
        // TODO Check for empty whitelist.
        manager->addNickname(nicknames[row]);
        newItem = false;
    }
    else {
        QLabel *policyLabel = new QLabel(policy);
        policyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row, 1, policyLabel);
        manager->updateNickname(nicknames[row]);
    }
    ui->addNicknameButton->setEnabled(true);
    ui->nicknameTableWidget->setFocus();

}

/*
 * Invoked from key filter signal on enter or return pressed.
 */
void NicknameHandler::policySelected() {

    QString value = policyComboBox->currentText();
    QString policy = getPolicy(value);
    policyEdited(policy);

}

void NicknameHandler::setNicknames(const Pippip::NicknameList& nicks) {

    nicknames = nicks;
    loadTable();

}

void NicknameHandler::whitelistChanged(const Pippip::EntityList &list) {

    int row = ui->nicknameTableWidget->currentRow();
    nicknames[row].whitelist = list;
    manager->updateNickname(nicknames[row]);

}

