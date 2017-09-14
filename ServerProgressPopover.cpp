#include "ServerProgressPopover.h"
#include "ui_ServerProgressPopover.h"
#include <QTimer>

/**
 * @brief ServerProgressPopover::ServerProgressPopover
 * @param parent
 */
ServerProgressPopover::ServerProgressPopover(QWidget *parent)
: QDialog(parent),
  ui(new Ui::ServerProgressPopover),
  finished(false),
  increments(10) {

    ui->setupUi(this);

}

/**
 * @brief ServerProgressPopover::~ServerProgressPopover
 */
ServerProgressPopover::~ServerProgressPopover() {

    delete ui;

}

/**
 * @brief ServerProgressPopover::done
 * @param res
 */
void ServerProgressPopover::done(int res) {

    finished = true;
    QDialog::done(res);

}

/**
 * @brief ServerProgressPopover::setInfo1
 * @param info
 */
void ServerProgressPopover::setInfo1(const QString &info) {

    ui->info1Label->setText(info);

}

/**
 * @brief ServerProgressPopover::setInfo2
 * @param info
 */
void ServerProgressPopover::setInfo2(const QString &info) {

    ui->info2Label->setText(info);

}

/**
 * @brief ServerProgressPopover::setTimeEstimate
 * @param estimate
 */
void ServerProgressPopover::setTimeEstimate(int estimate) {

    increments = 100 / estimate;

}

/**
 * @brief ServerProgressPopover::show
 */
void ServerProgressPopover::show() {

    QTimer::singleShot(1000, this, SLOT(updateProgress()));
    QDialog::show();

}

/**
 * @brief ServerProgressPopover::updateProgress
 */
void ServerProgressPopover::updateProgress() {

    if (!finished) {
        QTimer::singleShot(1000, this, SLOT(updateProgress()));
        int value = ui->progressBar->value();
        int newValue = std::min(value + increments, 100);
        ui->progressBar->setValue(newValue);
        qApp->processEvents();
    }

}
