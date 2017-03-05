#include "NewAccountDialog.h"
#include "ui_NewAccountDialog.h"

NewAccountDialog::NewAccountDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewAccountDialog)
{
    ui->setupUi(this);
}

NewAccountDialog::~NewAccountDialog()
{
    delete ui;
}
