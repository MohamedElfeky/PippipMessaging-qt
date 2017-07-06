#ifndef EMPTYSTRINGVALIDATOR_H
#define EMPTYSTRINGVALIDATOR_H

#include <QRegularExpressionValidator>

class EmptyStringValidator : public QRegularExpressionValidator {
        Q_OBJECT

    public:
        explicit EmptyStringValidator(QObject *parent = 0);
        EmptyStringValidator(const QRegularExpression& re, QObject *parent = 0);
        ~EmptyStringValidator() {}

    public:
        State validate(QString &input, int &pos) const;

};

#endif // EMPTYSTRINGVALIDATOR_H
