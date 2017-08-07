#include "EmptyStringValidator.h"
#include <QDebug>

EmptyStringValidator::EmptyStringValidator(QObject *parent)
: QRegularExpressionValidator(parent) {
}

EmptyStringValidator::EmptyStringValidator(const QRegularExpression &re, QObject *parent)
: QRegularExpressionValidator(re, parent) {
}

QValidator::State EmptyStringValidator::validate(QString &input, int &pos) const {

    if (input.length() == 0) {
        qDebug() << "Validator returning acceptable on empty string";
        return Acceptable;
    }
    else {
        return QRegularExpressionValidator::validate(input, pos);
    }

}
