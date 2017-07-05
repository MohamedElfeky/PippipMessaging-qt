#include "NicknamesDialog.h"
#include "ui_NicknamesDialog.h"
#include "SessionState.h"
#include "NicknameManager.h"
#include "CriticalAlert.h"
#include "EditWhitelistDialog.h"
#include "EnterKeyFilter.h"
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <assert.h>
#include <iostream>

static const QString POLICIES[] = { "Public", "FriendsOnly", "FriendsOfFriends" };
static const QString POLICY_NAMES[] = { "Public", "Friends", "Friends of Friends" };
static const QString LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

NicknamesDialog::NicknamesDialog(Pippip::SessionState *sess, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog),
  state(sess),
  newItem(false) {
  //newColumnCount(0) {

    ui->setupUi(this);

    QStringList headers;
    headers << "Nickname" << "Policy";
    // Set header labels
    ui->nicknameTableWidget->setHorizontalHeaderLabels(headers);
    // Set columns sizes to fill the table width.
    ui->nicknameTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Always select the whole row.
    ui->nicknameTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Single selection only.
    ui->nicknameTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNickname()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteNickname()));
    connect(ui->nicknameTableWidget, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(editPolicy(int, int)));
    connect(ui->nicknameTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(nicknameSelected()));
    connect(ui->nicknameTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(itemChanged(QTableWidgetItem*)));

    nicknameRE.setPattern("[A-z]+[A-z0-9_\\-]{5,}");
    nicknameRE.setPatternOptions(QRegularExpression::OptimizeOnFirstUsageOption);
    nicknameValidator = new QRegularExpressionValidator(nicknameRE, this);

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::addNickname() {

    newItem = true;
    ui->addButton->setEnabled(false);
    int editRow = ui->nicknameTableWidget->rowCount();
    ui->nicknameTableWidget->setRowCount(editRow + 1);
    nicknameLineEdit = new QLineEdit;
    nicknameLineEdit->setValidator(nicknameValidator);
    connect(nicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEdited()));
    ui->nicknameTableWidget->setCellWidget(editRow, 0, nicknameLineEdit);
/*    newColumnCount = 2;
    QTableWidgetItem *nicknameItem = new QTableWidgetItem;
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->nicknameTableWidget->setItem(rows, 0, nicknameItem);
    QTableWidgetItem *policyItem = new QTableWidgetItem(POLICIES[0]);
    policyItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->nicknameTableWidget->setItem(rows, 1, policyItem);*/
    policyComboBox = new QComboBox(this);
    QStringList items;
    items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
    policyComboBox->addItems(items);
    connect(policyComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(policyChanged(QString)));
    EnterKeyFilter *keyFilter = new EnterKeyFilter(this);
    policyComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(enterPressed()), this, SLOT(policySelected()));
    ui->nicknameTableWidget->setCellWidget(editRow, 1, policyComboBox);

    Pippip::Nickname newNick;
    //newNick.policy = POLICIES[0];
    nicknames.push_back(newNick);
    nicknameLineEdit->setFocus();
    //ui->nicknameTableWidget->editItem(nicknameItem);

}

void NicknamesDialog::deleteNickname() {

    int row = ui->nicknameTableWidget->currentRow();
    QTableWidgetItem *item = ui->nicknameTableWidget->item(row, 0);
    QString name = item->text();
    manager->deleteNickname(name);

}

void NicknamesDialog::editPolicy(int row, int /* column */) {

    //editRow = row;
    ui->addButton->setEnabled(false);
    policyComboBox = new QComboBox(this);
    QStringList items;
    items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
    policyComboBox->addItems(items);
    connect(policyComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(policyChanged(QString)));
    EnterKeyFilter *keyFilter = new EnterKeyFilter(this);
    policyComboBox->installEventFilter(keyFilter);
    connect(keyFilter, SIGNAL(enterPressed()), this, SLOT(policySelected()));
    ui->nicknameTableWidget->setCellWidget(row, 1, policyComboBox);

}

const QString& NicknamesDialog::getPolicy(const QString& name) const {

    for (int i = 0; i < 3; ++i) {
        if (name == POLICY_NAMES[i]) {
            return POLICIES[i];
        }
    }
    assert(false);

}

const QString& NicknamesDialog::getPolicyName(const QString& policy) const {

    for (int i = 0; i < 3; ++i) {
        if (policy == POLICIES[i]) {
            return POLICY_NAMES[i];
        }
    }
    assert(false);

}
/*
void NicknamesDialog::itemChanged(QTableWidgetItem *item) {

    editItem = item;
    int row = editItem->row();
    int column = editItem->column();
    QString value = editItem->text();


    if (newColumnCount-- == 0) {
        if (column == 0 && validateNickname(value)) {
            nicknames[row].entity.nickname = value;
            if (newItem) {
                ui->nicknameTableWidget->selectRow(row);
                policyComboBox->setFocus();
            }
        }
        else {
            Pippip::CriticalAlert alert("Invalid Nickname",
                                        "Invalid nickname, please choose another",
                                        "Nicknames must be at least five characters long and must begin with an upper or lower case letter");
            alert.exec();
        }
    }

}
*/
void NicknamesDialog::loadTable() {

    ui->nicknameTableWidget->clearContents();
    ui->nicknameTableWidget->setRowCount(nicknames.size());
    int row = 0;
    for (auto nickname : nicknames) {
        QLabel *nameLabel = new QLabel(nickname.entity.nickname);
        nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row, 0, nameLabel);
        QLabel *policyLabel = new QLabel(nickname.policy);
        policyLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row, 1, policyLabel);
/*        QTableWidgetItem *nicknameItem = new QTableWidgetItem(nickname.entity.nickname);
        nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        nicknameItem->setFlags(nicknameItem->flags() & ~Qt::ItemIsEditable);
        ui->nicknameTableWidget->setItem(row, 0, nicknameItem);
        QTableWidgetItem *policyItem = new QTableWidgetItem(getPolicyName(nickname.policy));
        policyItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        policyItem->setFlags(policyItem->flags() & ~Qt::ItemIsEditable);
        ui->nicknameTableWidget->setItem(row++, 1, policyItem);*/
    }
    ui->addButton->setEnabled(true);
}

void NicknamesDialog::nicknameAdded(const QString& name) {

    nicknames = manager->getNicknames();
    loadTable();
    for (int row = 0; row < nicknames.size(); ++row) {
        if (nicknames[row].entity.nickname == name) {
            ui->nicknameTableWidget->selectRow(row);
        }
    }
    ui->statusLabel->setText(name + " added");
    qApp->processEvents();

}

void NicknamesDialog::nicknameDeleted(const QString& name) {

    nicknames = manager->getNicknames();
    loadTable();
    ui->statusLabel->setText(name + " deleted");
    qApp->processEvents();

}

void NicknamesDialog::nicknameEdited() {

    int row = ui->nicknameTableWidget->currentRow();
    QString nickname = nicknameLineEdit->text();
    nicknames[row].entity.nickname = nickname;
    QLabel *nameLabel = new QLabel(nickname);
    nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->nicknameTableWidget->setCellWidget(row, 0, nameLabel);
    policyComboBox->setFocus();

}

void NicknamesDialog::nicknameSelected() {

    ui->deleteButton->setEnabled(true);

}

void NicknamesDialog::nicknamesLoaded(const Pippip::NicknameList& list) {

    nicknames = list;
    loadTable();
    if (nicknames.size() > 0) {
        ui->nicknameTableWidget->selectRow(0);
    }
    ui->statusLabel->setText("Nicknames loaded");
    qApp->processEvents();

}

void NicknamesDialog::nicknameUpdated(Pippip::Nickname nickname) {

    QList<QTableWidgetItem*> items =
            ui->nicknameTableWidget->findItems(nickname.entity.nickname, Qt::MatchExactly);
    assert(items.count() > 0);
    QTableWidgetItem *item = items.first();
    int row = item->row();
    ui->nicknameTableWidget->item(row, 1)->setText(getPolicyName(nickname.policy));
    ui->statusLabel->setText(nickname.entity.nickname + " updated");
    qApp->processEvents();

}

/*
 * Signaled when policy combo box selection changes.
 */
void NicknamesDialog::policyChanged(const QString& value) {

    QString policy = getPolicy(value);
    policyEdited(policy);

}

void NicknamesDialog::policyEdited(const QString &policy) {

    int row = ui->nicknameTableWidget->currentRow();
    if (policy != POLICIES[0]) {  // Public policy
        EditWhitelistDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            nicknames[row].whitelist = dialog.getWhitelist();
        }
    }

    nicknames[row].policy = policy;
    ui->nicknameTableWidget->removeCellWidget(row, 1);

    if (newItem) {
        // TODO Check for empty whitelist.
        manager->addNickname(nicknames[row]);
        newItem = false;
    }
    else {
        manager->updateNickname(nicknames[row]);
    }
    ui->addButton->setEnabled(true);
    ui->nicknameTableWidget->setFocus();

}

/*
 * Invoked from key filter signal on enter or return pressed.
 */
void NicknamesDialog::policySelected() {

    QString value = policyComboBox->currentText();
    QString policy = getPolicy(value);
    policyEdited(policy);

}

void NicknamesDialog::requestFailed(const QString &reqName, const QString& error) {

    Pippip::CriticalAlert alert(reqName + " Failed", "Unable to process request", error);
    alert.exec();
    nicknames = manager->getNicknames();
    loadTable();

}

void NicknamesDialog::setManager(Pippip::NicknameManager *man) {

    manager = man;
    connect(manager, SIGNAL(nicknamesLoaded(const Pippip::NicknameList&)),
                                    this, SLOT(nicknamesLoaded(const Pippip::NicknameList&)));
    connect(manager, SIGNAL(nicknameDeleted(QString)), this, SLOT(nicknameDeleted(QString)));
    connect(manager, SIGNAL(nicknameUpdated(Pippip::Nickname)),
                                                this, SLOT(nicknameUpdated(Pippip::Nickname)));
    connect(manager, SIGNAL(nicknameAdded(QString)), this, SLOT(nicknameAdded(QString)));
    connect(manager, SIGNAL(requestFailed(QString, QString)), this, SLOT(requestFailed(QString, QString)));

    manager->loadNicknames();

}
/*
bool NicknamesDialog::validateNickname(const QString &nickname) const {

    if (nickname.length() < 5) {
        return false;
    }

    if (LETTERS.indexOf(nickname[0]) < 0) {
        return false;
    }

    return true;

}
*/
