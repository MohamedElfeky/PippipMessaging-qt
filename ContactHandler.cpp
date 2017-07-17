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
#include "ContactManager.h"
#include "SessionState.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>

ContactHandler::ContactHandler(Ui::ContactsDialog *u, Pippip::SessionState *s, QObject *parent)
: QObject(parent),
  ui(u),
  state(s) {

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(requestContact()));

    nicknameRE.setPattern("[A-z]+[A-z0-9_\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new EmptyStringValidator(nicknameRE, this);

    puidRE.setPattern("[1-9a-f]+[0-9a-f]{19,}");
    puidRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    puidValidator = new EmptyStringValidator(puidRE, this);

}

void ContactHandler::contactRequested(const QString &name) {

    contactManager->loadContacts();
    ui->contactsStatusLabel->setText("Contact with " + name + " requested");
    qApp->processEvents();

}

void ContactHandler::contactsLoaded() {

    contacts = contactManager->getContacts();
    loadTable();
    if (contacts.size() > 0) {
        ui->contactsTableWidget->selectRow(0);
    }
    ui->contactsStatusLabel->setText("Contacts loaded");
    qApp->processEvents();

}

void ContactHandler::loadTable() {

    ui->contactsTableWidget->clearContents();
    ui->contactsTableWidget->setRowCount(contacts.size());

    for (unsigned row = 0; row < contacts.size(); row++) {
        const Pippip::Contact& contact = contacts[row];
        QLabel *statusLabel = new QLabel(contact.status);
        statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        statusLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row, 0, statusLabel);

        QLabel *requestingLabel = new QLabel(contact.contactOf);
        requestingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        requestingLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row, 1, requestingLabel);

        QLabel *nicknameLabel = new QLabel(contact.entity.nickname);
        nicknameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        nicknameLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row, 2, nicknameLabel);

        QLabel *puidLabel = new QLabel(contact.entity.publicId);
        puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        puidLabel->setMargin(5);
        ui->contactsTableWidget->setCellWidget(row, 3, puidLabel);
    }

}

/*
 * Invoked by the nickname line edit editing finished signal.
 */
void ContactHandler::nicknameEdited() {

    disconnect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    int row = ui->contactsTableWidget->currentRow();
    nicknameLineEdit->setEnabled(false);
    QLabel *nicknameLabel = new QLabel(nicknameLineEdit->text());
    nicknameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    nicknameLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(row, 2, nicknameLabel);
    working.requested.nickname = nicknameLineEdit->text();

    puidLineEdit = new QLineEdit;
    puidLineEdit->setValidator(puidValidator);
    ui->contactsTableWidget->setCellWidget(row, 3, puidLineEdit);
    puidLineEdit->setFocus();
    connect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);

}

/*
 * Invoked by the public ID line edit editing finished signal.
 */
void ContactHandler::puidEdited() {

    disconnect(puidLineEdit, SIGNAL(editingFinished()), this, SLOT(puidEdited()));
    int row = ui->contactsTableWidget->currentRow();
    puidLineEdit->setEnabled(false);
    QString publicId = puidLineEdit->text();
    QLabel *puidLabel = new QLabel(publicId);
    puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    puidLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(row, 3, puidLabel);
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->addButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);

    working.requested.publicId = puidLineEdit->text();
    contactManager->requestContact(working);

}

void ContactHandler::requestContact() {

    working.requesting.publicId = state->publicId;
    working.privateRequest = false;

    ui->addButton->setEnabled(false);
    int row = ui->contactsTableWidget->rowCount();
    ui->contactsTableWidget->setRowCount(row + 1);

    QLabel *statusLabel = new QLabel("pending");
    statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    statusLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(row, 0, statusLabel);

    requestingComboBox = new QComboBox;
    QStringList items;
    for (auto nickname : nicknames) {
        items << nickname.entity.nickname;
    }
    requestingComboBox->addItems(items);
    EnterKeyFilter *keyFilter = new EnterKeyFilter(requestingComboBox);
    requestingComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(enterPressed()), this, SLOT(requestingSelected()));
    ui->contactsTableWidget->setCellWidget(row, 1, requestingComboBox);
    // Do this here so the event doesn't fire when the value is changed above.
    connect(requestingComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(requestingSet(QString)));
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);
    requestingComboBox->setFocus();

}

/*
 * Invoked by key filter signal on enter or return pressed.
 */
void ContactHandler::requestingSelected() {

    QString nickname = requestingComboBox->currentText();
    requestingSet(nickname);

}

/*
 * Invoked by the combo box selection change signal or directly from
 * nicknameSelected.
 */
void ContactHandler::requestingSet(const QString &nick) {

    int row = ui->contactsTableWidget->currentRow();
    QLabel *requestingLabel = new QLabel(nick);
    requestingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    requestingLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(row, 1, requestingLabel);
    working.requesting.nickname = nick;

    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    ui->contactsTableWidget->setCellWidget(row, 2, nicknameLineEdit);
    nicknameLineEdit->setFocus();
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->pastePushButton->setEnabled(true);
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);

}

void ContactHandler::setNicknames(const Pippip::NicknameList &nicks) {

    nicknames = nicks;
    ui->addButton->setEnabled(true);

}
