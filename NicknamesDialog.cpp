#include "NicknamesDialog.h"
#include "EditNicknameDialog.h"
#include "ui_NicknamesDialog.h"
#include "SessionState.h"
#include <QTableWidgetItem>

// Keep the policy indexes in sync
const int NicknamesDialog::PRIVATE = EditNicknameDialog::PRIVATE;
const int NicknamesDialog::PUBLIC = EditNicknameDialog::PUBLIC;
const int NicknamesDialog::FRIENDSONLY = EditNicknameDialog::FRIENDSONLY;
const int NicknamesDialog::FRIEDSOFFRIENDS = EditNicknameDialog::FRIEDSOFFRIENDS;

static QString policyNames[] = { "Private", "Public", "Friends Only", "Friends of Friends" };

NicknamesDialog::NicknamesDialog(Pippip::SessionState *sess, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog),
  state(sess) {

    ui->setupUi(this);
    QStringList headers;
    headers << "Nickname" << "Policy";
    ui->nicknameTableWidget->setShowGrid(false);
    ui->nicknameTableWidget->setColumnCount(2);
    ui->nicknameTableWidget->setHorizontalHeaderLabels(headers);
    ui->nicknameTableWidget->verticalHeader()->setVisible(false);
    ui->nicknameTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->nicknameTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNickname()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteNickname()));

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::addNickname() {

    EditNicknameDialog editDialog;
    if (editDialog.exec() == QDialog::Accepted) {
        Nickname nickname;
        nickname.nickname = editDialog.getNickname();
        nickname.policy = editDialog.getPolicy();
        nicknames.push_back(nickname);
        int rows = ui->nicknameTableWidget->rowCount();
        ui->nicknameTableWidget->setRowCount(rows + 1);
        ui->nicknameTableWidget->setItem(rows, 0, new QTableWidgetItem(nickname.nickname));
        ui->nicknameTableWidget->setItem(rows, 1, new QTableWidgetItem(policyNames[nickname.policy]));
    }

}

void NicknamesDialog::deleteNickname() {

    int row = ui->nicknameTableWidget->currentRow();
    QString nickname = nicknames[row].nickname;
    NicknameList tmp;
    for (Nickname nick : nicknames) {
        if (nick.nickname != nickname) {
            tmp.push_back(nick);
        }
    }
    nicknames.swap(tmp);
    ui->nicknameTableWidget->removeRow(row);

}

void NicknamesDialog::editNickname() {
/*
    int row = index.row();
    Nickname& nickname = nicknames[row];
    EditNicknameDialog editDialog;
    editDialog.setNickname(nickname.nickname);
    editDialog.setPolicy(nickname.policy);
    if (editDialog.exec() == QDialog::Accepted) {
        nickname.policy = editDialog.getPolicy();
        QString line = nickname.nickname;
        line.append(" - ").append(policyNames[nickname.policy]);
        ui->nicknameListWidget->currentItem()->setText(line);
    }
*/
}

void NicknamesDialog::setNicknames(const NicknameList& list) {
    nicknames = list;
}
