#include "EditNicknameDialog.h"
#include "ui_EditNicknameDialog.h"

const int EditNicknameDialog::PRIVATE = 0;
const int EditNicknameDialog::PUBLIC = 1;
const int EditNicknameDialog::FRIENDSONLY = 2;
const int EditNicknameDialog::FRIENDSOFFRIENDS = 3;

EditNicknameDialog::EditNicknameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditNicknameDialog) {

    ui->setupUi(this);
    ui->nicknameEdit->setFocus();

}

EditNicknameDialog::~EditNicknameDialog()
{
    delete ui;
}

void EditNicknameDialog::accept() {

    nickname = ui->nicknameEdit->text();
    policy = ui->policyComboBox->currentIndex();

    QDialog::accept();

}

void EditNicknameDialog::setNickname(const QString &nick) {

    ui->nicknameEdit->setText(nick);
    // Setting the nickname means we're just editing the policy.
    ui->nicknameEdit->setDisabled(true);

}

void EditNicknameDialog::setPolicy(int policyValue) {

    ui->policyComboBox->setCurrentIndex(policyValue);

}
