#include "NewAccountHelpDialog.h"
#include "ui_NewAccountHelpDialog.h"

NewAccountHelpDialog::NewAccountHelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAccountHelpDialog)
{
    ui->setupUi(this);
}

NewAccountHelpDialog::~NewAccountHelpDialog()
{
    delete ui;
}
