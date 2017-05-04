#include "NicknamesDialog.h"
#include "EditNicknameDialog.h"
#include "ui_NicknamesDialog.h"
#include "SessionState.h"
#include <QTableWidgetItem>
#include <QHeaderView>

// Keep the policy indexes in sync
const int NicknamesDialog::PRIVATE = EditNicknameDialog::PRIVATE;
const int NicknamesDialog::PUBLIC = EditNicknameDialog::PUBLIC;
const int NicknamesDialog::FRIENDSONLY = EditNicknameDialog::FRIENDSONLY;
const int NicknamesDialog::FRIENDSOFFRIENDS = EditNicknameDialog::FRIENDSOFFRIENDS;

static QString policyNames[] = { "Private", "Public", "Friends Only", "Friends of Friends" };

NicknamesDialog::NicknamesDialog(Pippip::SessionState *sess, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog),
  state(sess) {

    ui->setupUi(this);
    QStringList headers;
    headers << "Nickname" << "Policy";
    // Turn off the grid.
    ui->nicknameTableWidget->setShowGrid(false);
    // Always 2 columns
    ui->nicknameTableWidget->setColumnCount(2);
    // Set header labels
    ui->nicknameTableWidget->setHorizontalHeaderLabels(headers);
    // Set columns sizes to fill the table width.
    ui->nicknameTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // No vertical headers
    ui->nicknameTableWidget->verticalHeader()->setVisible(false);
    // Always select the whole row.
    ui->nicknameTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    // Single selection only.
    ui->nicknameTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    // Populate the table widget.
    setNicknames();

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNickname()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteNickname()));
    connect(ui->nicknameTableWidget, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(editNickname(QModelIndex)));

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::addNickname() {

    EditNicknameDialog editDialog;
    if (editDialog.exec() == QDialog::Accepted) {
        Pippip::Nickname nickname;
        nickname.nickname = editDialog.getNickname();
        nickname.policy = editDialog.getPolicy();
        state->nicknames.push_back(nickname);
        int rows = ui->nicknameTableWidget->rowCount();
        ui->nicknameTableWidget->setRowCount(rows + 1);
        addNickname(nickname, rows);
    }

}

void NicknamesDialog::addNickname(const Pippip::Nickname& nickname, int row) {

    QTableWidgetItem *nicknameItem = new QTableWidgetItem(nickname.nickname);
    nicknameItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    nicknameItem->setFlags(nicknameItem->flags() & ~Qt::ItemIsEditable);
    ui->nicknameTableWidget->setItem(row, 0, nicknameItem);
    QTableWidgetItem *policyItem = new QTableWidgetItem(policyNames[nickname.policy]);
    policyItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    policyItem->setFlags(nicknameItem->flags() & ~Qt::ItemIsEditable);
    ui->nicknameTableWidget->setItem(row, 1,policyItem);

}

void NicknamesDialog::deleteNickname() {

    QList<QTableWidgetItem*> items = ui->nicknameTableWidget->selectedItems();
    if (items.count() > 0) {
        int row = items.first()->row();
        QString nickname = state->nicknames[row].nickname;
        Pippip::NicknameList tmp;
        for (Pippip::Nickname nick : state->nicknames) {
            if (nick.nickname != nickname) {
                tmp.push_back(nick);
            }
        }
        state->nicknames.swap(tmp);
        ui->nicknameTableWidget->removeRow(row);
    }

}

void NicknamesDialog::editNickname(QModelIndex index) {

    int row = index.row();
    Pippip::Nickname& nickname = state->nicknames[row];
    EditNicknameDialog editDialog;
    editDialog.setNickname(nickname.nickname);  // Disables nickname editing.
    editDialog.setPolicy(nickname.policy);
    if (editDialog.exec() == QDialog::Accepted) {
        nickname.policy = editDialog.getPolicy();
        ui->nicknameTableWidget->item(row, 1)->setText(policyNames[nickname.policy]);
    }

}

void NicknamesDialog::setNicknames() {

    int rows = state->nicknames.size();
    ui->nicknameTableWidget->setRowCount(rows);
    int row = 0;
    for (Pippip::Nickname nickname : state->nicknames) {
        addNickname(nickname, row);
        row++;
    }

}
