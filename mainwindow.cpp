#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SessionState.h"
#include "NewAccountDialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    session(new Pippip::SessionState(this)) {

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

    NewAccountDialog dialog(session);
    dialog.exec();

}
