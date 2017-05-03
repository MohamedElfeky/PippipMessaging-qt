#include "NicknamesDialog.h"
#include "EditNicknameDialog.h"
#include "ui_NicknamesDialog.h"
#include "NicknameModel.h"
#include "SessionState.h"
#include <QStringListModel>

NicknamesDialog::NicknamesDialog(Pippip::SessionState *sess, QWidget *parent)
: QDialog(parent),
  ui(new Ui::NicknamesDialog),
  state(sess) {

    ui->setupUi(this);
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addNickname()));
    nicknames = new NicknameModel(this);
    ui->nicknameListView->setModel(nicknames);

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::addNickname() {
/*
    EditNicknameDialog editDialog;
    if (editDialog.exec() == QDialog::Accepted) {
        Nickname nickname;
        nickname.nickname = editDialog.getNickname();
        nickname.policy = editDialog.getPolicy();
        nicknames.push_back(nickname);
        QListWidgetItem *item = new QListWidgetItem(ui->nicknameListWidget);
        QString line = nickname.nickname;
        line.append(" - ").append(policyNames[nickname.policy]);
        item->setText(line);
        ui->nicknameListWidget->addItem(item);
    }
*/
}

void NicknamesDialog::deleteNickname() {
}

void NicknamesDialog::editNickname(QModelIndex index) {
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
