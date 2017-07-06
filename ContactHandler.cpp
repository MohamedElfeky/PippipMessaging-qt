/*
 * ContactHandler.cpp
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

#include "ContactHandler.h"
#include "ui_ContactsDialog.h"
#include "EnterKeyFilter.h"
#include "EmptyStringValidator.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>

ContactHandler::ContactHandler(Ui::ContactsDialog *u, QObject *parent)
: QObject(parent),
  ui(u) {

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(requestContact()));

    nicknameRE.setPattern("[A-z]+[A-z0-9_\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new EmptyStringValidator(nicknameRE, this);

    puidRE.setPattern("[1-9a-f]+[0-9a-f]{19,}");
    puidRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    puidValidator = new EmptyStringValidator(puidRE, this);

}

void ContactHandler::nicknameEdited() {

    int row = ui->contactsTableWidget->currentRow();
    QLabel *nicknameLabel = new QLabel(nicknameLineEdit->text());
    nicknameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setCellWidget(row, 2, nicknameLabel);

    puidLineEdit = new QLineEdit;
    puidLineEdit->setValidator(puidValidator);
    connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
    ui->contactsTableWidget->setCellWidget(row, 2, puidLineEdit);
    puidLineEdit->setFocus();

}

void ContactHandler::nicknameSet(const QString &nick) {

    int row = ui->contactsTableWidget->currentRow();
    QLabel *nicknameLabel = new QLabel(nick);
    nicknameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setCellWidget(row, 1, nicknameLabel);

    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->contactsTableWidget->setCellWidget(row, 2, nicknameLineEdit);
    nicknameLineEdit->setFocus();
    ui->pastePushButton->setEnabled(true);

}

/*
 * Invoked from key filter signal on enter or return pressed.
 */
void ContactHandler::nicknameSelected() {

    QString nickname = nicknameComboBox->currentText();
    nicknameSet(nickname);

}

void ContactHandler::puidEdited() {

    int row = ui->contactsTableWidget->currentRow();
    QString publicId = puidLineEdit->text();
    QLabel *puidLabel = new QLabel(publicId);
    puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setCellWidget(row, 3, puidLabel);

    ui->addButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);

}

void ContactHandler::requestContact() {

    ui->addButton->setEnabled(false);
    int row = ui->contactsTableWidget->rowCount();
    ui->contactsTableWidget->setRowCount(row + 1);

    QLabel *statusLabel = new QLabel("pending");
    statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->contactsTableWidget->setCellWidget(row, 0, statusLabel);

    nicknameComboBox = new QComboBox;
    QStringList items;
    for (auto nickname : nicknames) {
        items << nickname.entity.nickname;
    }
    nicknameComboBox->addItems(items);
    EnterKeyFilter *keyFilter = new EnterKeyFilter(nicknameComboBox);
    nicknameComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(enterPressed()), this, SLOT(nicknameSelected()));
    ui->contactsTableWidget->setCellWidget(row, 1, nicknameComboBox);
    // Do this here so the event doesn't fire when the value is changed above.
    connect(nicknameComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(nicknameSet(QString)));
    nicknameComboBox->setFocus();

}
