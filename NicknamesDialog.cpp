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

}

NicknamesDialog::~NicknamesDialog() {
    delete ui;
}

void NicknamesDialog::addNickname() {

    EditNicknameDialog editDialog;
    if (editDialog.exec() == QDialog::Accepted) {
        NicknameModel::Nickname nickname;
        nickname.nickname = editDialog.getNickname();
        nickname.policy = editDialog.getPolicy();
        nicknames->addNickname(nickname);
    }

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

void NicknamesDialog::setNicknames(NicknameModel *model) {

    nicknames = model;
    ui->nicknameTableView->setModel(model);
    //connect(nicknames, SIGNAL(dataChanged(QModelIndex,QModelIndex)), ui->nicknameTableView, SLOT(dataChanged(QModelIndex,QModelIndex)));

}
