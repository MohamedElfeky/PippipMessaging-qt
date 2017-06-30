#include "NicknamesDialog.h"
#include "ui_NicknamesDialog.h"
#include "SessionState.h"
#include "NicknameManager.h"
#include "CriticalAlert.h"
#include "EditWhitelistDialog.h"
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <assert.h>

static const QString POLICIES[] = { "Public", "FriendsOnly", "FriendsOfFriends" };
static const QString POLICY_NAMES[] = { "Public", "Friends", "Friends of Friends" };
static const QString LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

NicknamesDialog::NicknamesDialog(Pippip::SessionState *sess, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog),
  state(sess),
  newNickname(false) {

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
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteNicknames()));
    connect(ui->nicknameTableWidget, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(editPolicy(int, int)));
    connect(ui->nicknameTableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(nicknameSelected()));

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::addNickname() {

    newNickname = true;
    ui->addButton->setEnabled(false);
    int rows = ui->nicknameTableWidget->rowCount();
    ui->nicknameTableWidget->setRowCount(rows + 1);
    newNicknameLineEdit = new QLineEdit(this);
    editRow = rows;
    ui->nicknameTableWidget->setCellWidget(editRow, 0, newNicknameLineEdit);
    connect(newNicknameLineEdit, SIGNAL(editingFinished()), this, SLOT(nicknameEditComplete()));
    newNicknameLineEdit->setFocus();

}

void NicknamesDialog::deleteNicknames() {

    QList<QTableWidgetItem*> items = ui->nicknameTableWidget->selectedItems();
    for (QTableWidgetItem* item : items) {
        if (item->column() == 0) {
            QString name = item->text();
            manager->deleteNickname(name);
        }
    }

}

void NicknamesDialog::editPolicy(int row , int /* column */) {

    editRow = row;
    ui->addButton->setEnabled(false);
    newPolicyComboBox = new QComboBox(this);
    QStringList items;
    items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
    newPolicyComboBox->addItems(items);
    ui->nicknameTableWidget->setCellWidget(editRow, 1, newPolicyComboBox);
    connect(newPolicyComboBox, SIGNAL(activated(int)), this, SLOT(policyEditComplete(int)));
    newPolicyComboBox->setFocus();

}

QString NicknamesDialog::getPolicyName(QString policy) const {

    QString policyName;

    for (int i = 0; i < 3; ++i) {
        if (policy == POLICIES[i]) {
            policyName = POLICY_NAMES[i];
        }
    }
    return policyName;

}

void NicknamesDialog::nicknameAdded(QString name, QString policy) {

    int rows = ui->nicknameTableWidget->rowCount();
    ui->nicknameTableWidget->setRowCount(rows + 1);
    QTableWidgetItem *nicknameItem = new QTableWidgetItem(name);
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    nicknameItem->setFlags(nicknameItem->flags() & ~Qt::ItemIsEditable);
    ui->nicknameTableWidget->setItem(rows, 0, nicknameItem);
    QTableWidgetItem *policyItem = new QTableWidgetItem(getPolicyName(policy));
    policyItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    policyItem->setFlags(policyItem->flags() & ~Qt::ItemIsEditable);
    ui->nicknameTableWidget->setItem(rows, 1, policyItem);
    ui->statusLabel->setText(editedNickname + " added");
    qApp->processEvents();

}

void NicknamesDialog::nicknameDeleted(QString name) {

    QList<QTableWidgetItem*> items = ui->nicknameTableWidget->findItems(name, Qt::MatchExactly);
    assert(items.count() > 0);
    int row = items.first()->row();
    ui->nicknameTableWidget->removeRow(row);
    ui->statusLabel->setText(name + " deleted");
    qApp->processEvents();

}

void NicknamesDialog::nicknameEditComplete() {

    editedNickname = newNicknameLineEdit->text();
    if (validateNickname(editedNickname)) {
        QLabel *label = new QLabel(editedNickname, this);
        ui->nicknameTableWidget->setCellWidget(editRow, 0, label);
        if (newNickname) {
            newPolicyComboBox = new QComboBox(this);
            QStringList items;
            items << POLICY_NAMES[0] << POLICY_NAMES[1] << POLICY_NAMES[2];
            newPolicyComboBox->addItems(items);
            ui->nicknameTableWidget->setCellWidget(editRow, 1, newPolicyComboBox);
            connect(newPolicyComboBox, SIGNAL(activated(int)), this, SLOT(policyEditComplete(int)));
            newPolicyComboBox->setFocus();
        }
    }
    else {
        Pippip::CriticalAlert alert("Invalid Nickname",
                                    "Invalid nickname, please choose another",
                                    "Nicknames must be at least five characters long and must begin with an upper or lower case letter");
        alert.exec();
    }

}

void NicknamesDialog::nicknameSelected() {

    ui->deleteButton->setEnabled(true);

}

void NicknamesDialog::nicknamesLoaded() {

    const Pippip::NicknameList& nicknames = manager->getNicknames();
    //int rows = nicknames.size();
    ui->nicknameTableWidget->setRowCount(0);
    for (auto nickname : nicknames) {
        // nicknameAdded(nickname.entity.nickname, nickname.policy);
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

void NicknamesDialog::policyEditComplete(int item) {

    Pippip::Nickname nickname;
    editedPolicy = POLICIES[item];

    if (editedPolicy != POLICIES[0]) {  // Public policy
        EditWhitelistDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            nickname.whitelist = dialog.getWhitelist();
        }
    }

    nickname.entity.nickname = editedNickname;
    //nickname.entity.publicId = state->publicId;
    nickname.policy = editedPolicy;

    if (newNickname) {
        int rows = ui->nicknameTableWidget->rowCount();
        ui->nicknameTableWidget->setRowCount(rows - 1);
        //manager->addNickname(nickname);
        emit nicknameAdded(editedNickname, editedPolicy);
    }
    else {
        ui->nicknameTableWidget->removeCellWidget(editRow, 1);
        //manager->updateNickname(nickname);
        emit nicknameUpdated(nickname);
    }

    ui->addButton->setEnabled(true);

}

void NicknamesDialog::setManager(Pippip::NicknameManager *man) {

    manager = man;
    connect(manager, SIGNAL(nicknamesLoaded()), this, SLOT(nicknamesLoaded()));
    connect(manager, SIGNAL(nicknameDeleted(QString)), this, SLOT(nicknameDeleted(QString)));
    connect(manager, SIGNAL(nicknameUpdated(Pippip::Nickname)), this,
                                                SLOT(nicknameUpdated(Pippip::Nickname)));
    connect(manager, SIGNAL(nicknameAdded(QString, QString)), this,
                                                SLOT(nicknameAdded(QString,QString)));
/*
    if (!manager->isLoaded()) {
        manager->loadNicknames();
    }
    else {
        nicknamesLoaded();
    }
*/
}

bool NicknamesDialog::validateNickname(const QString &nickname) const {

    if (nickname.length() < 5) {
        return false;
    }

    if (LETTERS.indexOf(nickname[0]) < 0) {
        return false;
    }

    return true;

}
