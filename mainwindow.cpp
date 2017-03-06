#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NewAccountDialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);
    connect(ui->NewAccountAction, SIGNAL(triggered()), this, SLOT(newAccount()));
    connect(ui->LoginAction, SIGNAL(triggered()), this, SLOT(logIn()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::logIn() {

}

void MainWindow::newAccount() {

    NewAccountDialog dialog;
    dialog.exec();

}
