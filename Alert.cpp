#include "Alert.h"
#include <QString>

namespace Pippip {

Alert::Alert(const QString& title,
             const QString& text,
             const QString& detail,
             const QMessageBox::Icon icon,
             QObject *parent)
: QObject(parent) {

    message = new QMessageBox;
    message->addButton(QMessageBox::Ok);
    message->setWindowTitle(title);
    message->setText(text);
    if (detail.length() > 0) {
        message->setInformativeText(detail);
    }
    message->setIcon(icon);

}

Alert::~Alert() {

    delete message;

}

void Alert::exec() {

    message->exec();

}

}
