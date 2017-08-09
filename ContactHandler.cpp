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
#include "KeyFilter.h"
#include "ContactsDatabase.h"
#include "ContactManager.h"
#include "SessionState.h"
#include "Nicknames.h"
#include "Contacts.h"
#include "Constants.h"
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QRegularExpressionValidator>
#include <QDebug>

ContactHandler::ContactHandler(Ui::ContactsDialog *u, Pippip::SessionState *s, QObject *parent)
: QObject(parent),
  ui(u),
  state(s) {

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(requestContact()));

    nicknameRE.setPattern("[A-z]+[A-z0-9 _\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new QRegularExpressionValidator(nicknameRE, this);

    puidRE.setPattern("[1-9a-f]+[0-9a-f]{19,}");
    puidRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    puidValidator = new QRegularExpressionValidator(puidRE, this);

    requestHeadings << "Status" << "Requesting ID" << "Requested ID Type" << "Requested ID";
    contactHeadings << "Status" << "My Nickname" << "Contact Nickname" << "Contact Public ID";

}

void ContactHandler::addContactFailed(const QString &error) {

    ui->contactsStatusLabel->setText(Constants::REDX_ICON + "Add contact failed - " + error);
    qApp->processEvents();

}

void ContactHandler::checkButtons() {

    // All contacts can be deleted.
    ui->deleteButton->setEnabled(true);
    int row = ui->contactsTableWidget->currentRow();
    QLabel *statusLabel =
            dynamic_cast<QLabel*>(ui->contactsTableWidget->cellWidget(row, 0));
    QString status = statusLabel->text();
    ui->queryButton->setEnabled(status == "pending");
    ui->suspendButton->setEnabled(status == "active");

}

/*
 * Calculates the remaining column width for coumn 3.
 */
int ContactHandler::columnGeometry() const {

    int tableWidth = ui->contactsTableWidget->width();
    int columnWidths = ui->contactsTableWidget->columnWidth(0);
    columnWidths += ui->contactsTableWidget->columnWidth(1);
    columnWidths += ui->contactsTableWidget->columnWidth(2);

    return tableWidth - columnWidths;

}

void ContactHandler::contactRequestComplete() {

    loadTable();
    ui->addButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);
    ui->contactsStatusLabel->setText(Constants::CHECK_ICON + "Contact requested");
    qApp->processEvents();

}

void ContactHandler::contactRequestFailed(const QString &error) {

    loadTable();
    ui->addButton->setEnabled(true);
    ui->pastePushButton->setEnabled(false);
    ui->contactsStatusLabel->setText(Constants::REDX_ICON + "Contact request failed - "
                                     + error);
    qApp->processEvents();

}

/*
 * Invoked by key filter signal on enter or return pressed.
 */
void ContactHandler::idTypeSelected(Qt::Key) {

    QString type = idTypeComboBox->currentText();
    QLabel *requestedLabel = new QLabel(type);
    requestedLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    requestedLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 2, requestedLabel);
    if (type == "Public ID") {
        requestedType = "P";
    }
    else {
        requestedType = "N";
    }

    requestedIdLineEdit = new QLineEdit;
    if (requestedType == "N") {
        requestedIdLineEdit->setValidator(nicknameValidator);
    }
    else {
        requestedIdLineEdit->setValidator(puidValidator);
    }
    ui->contactsTableWidget->setCellWidget(0, 3, requestedIdLineEdit);
    connect(requestedIdLineEdit, SIGNAL(editingFinished()), this, SLOT(requestedIdEdited()));
    ui->pastePushButton->setEnabled(true);
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->contactsTableWidget->setColumnWidth(3, columnGeometry());
    //requestedIdLineEdit->setFixedWidth(sectionWidth);
    requestedIdLineEdit->setFocus();

}

void ContactHandler::loadTable(int startingRow) {

    ui->contactsTableWidget->clearContents();
    ui->contactsTableWidget->setRowCount(contacts->size() + startingRow);
    if (startingRow == 0) {
        ui->contactsTableWidget->setHorizontalHeaderLabels(contactHeadings);
    }

    int row = startingRow;
    for (const auto contact : *contacts) {
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
        ui->contactsTableWidget->setCellWidget(row++, 3, puidLabel);
    }

    if (startingRow == 0 && contacts->size() > 0) {
        ui->contactsTableWidget->selectRow(0);
    }

}

void ContactHandler::requestContact() {

    ui->addButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->suspendButton->setEnabled(false);
    ui->queryButton->setEnabled(false);
    loadTable(1);   // Starting at row 1.
    ui->contactsTableWidget->setHorizontalHeaderLabels(requestHeadings);

    QLabel *statusLabel = new QLabel("pending");
    statusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    statusLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 0, statusLabel);

    requestingIdComboBox = new QComboBox;
    QStringList items;
    for (auto nickname : *nicknames) {
        items << nickname.entity.nickname;
    }
    items << "Public ID";
    requestingIdComboBox->addItems(items);
    KeyFilter *keyFilter = new KeyFilter(requestingIdComboBox);
    keyFilter->addKey(Qt::Key_Enter);
    keyFilter->addKey(Qt::Key_Return);
    keyFilter->addKey(Qt::Key_Tab);
    requestingIdComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(keyPressed(Qt::Key)), this, SLOT(requestingIdSelected(Qt::Key)));
    ui->contactsTableWidget->setCellWidget(0, 1, requestingIdComboBox);
    // Do this here so the event doesn't fire when the value is changed above.
    //connect(requestingComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(requestingSet(QString)));
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);
    requestingIdComboBox->setFocus();

}

/*
 * Invoked by the requested ID line edit editing finished signal.
 */
void ContactHandler::requestedIdEdited() {

    requestedIdLineEdit->setEnabled(false);
    QLabel *requestedIdLabel = new QLabel(requestedIdLineEdit->text());
    requestedIdLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    requestedIdLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 3, requestedIdLabel);
    workingRequest.requestedId = requestedIdLineEdit->text();
    workingRequest.idTypes = requestingType + requestedType;

    contactsDatabase->requestContact(workingRequest);

}

/*
 * Invoked by key filter signal on enter or return pressed.
 */
void ContactHandler::requestingIdSelected(Qt::Key) {

    QString id = requestingIdComboBox->currentText();
    QLabel *requestingLabel = new QLabel(id);
    requestingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    requestingLabel->setMargin(5);
    ui->contactsTableWidget->setCellWidget(0, 1, requestingLabel);
    if (id == "Public ID") {
        workingRequest.requestingId = state->publicId;
        requestingType = "P";
    }
    else {
        workingRequest.requestingId = id;
        requestingType = "N";
    }


    idTypeComboBox = new QComboBox;
    QStringList items;
    items << "Nickname" << "Public ID";
    idTypeComboBox->addItems(items);
    KeyFilter *keyFilter = new KeyFilter(idTypeComboBox);
    keyFilter->addKey(Qt::Key_Enter);
    keyFilter->addKey(Qt::Key_Return);
    keyFilter->addKey(Qt::Key_Tab);
    idTypeComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(keyPressed(Qt::Key)), this, SLOT(idTypeSelected(Qt::Key)));
    ui->contactsTableWidget->setCellWidget(0, 2, idTypeComboBox);
    // Do this here so the event doesn't fire when the value is changed above.
    //connect(requestedComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(requestedSet(QString)));
    ui->contactsTableWidget->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    ui->contactsTableWidget->horizontalHeader()->setStretchLastSection(true);
    idTypeComboBox->setFocus();

}

void ContactHandler::setContactsDatabase(Pippip::ContactsDatabase *database) {

    contactsDatabase = database;
    Pippip::ContactManager *contactManager = contactsDatabase->getContactManager();
    connect(contactManager, SIGNAL(contactRequestFailed(QString)),
            this, SLOT(contactRequestFailed(QString)));
    connect(contactsDatabase, SIGNAL(addContactFailed(QString)), this, SLOT(addContactFailed(QString)));
    contacts = contactsDatabase->getContacts();
    ui->contactsStatusLabel->setText(Constants::CHECK_ICON + "Contacts loaded");
    qApp->processEvents();

}

void ContactHandler::setNicknames(Pippip::Nicknames *nicks) {

    nicknames = nicks;
    ui->addButton->setEnabled(true);

}
