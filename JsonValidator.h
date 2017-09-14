#ifndef JSONVALIDATOR_H
#define JSONVALIDATOR_H

#include <QJsonObject>
#include <QJsonValue>
#include <QString>

namespace Pippip {

class JsonValidator {

    public:
        JsonValidator(const QJsonObject& obj, const QString& key);
        ~JsonValidator() {}

    private:
        JsonValidator(const JsonValidator& other);
        JsonValidator& operator =(const JsonValidator& other);

    public:
        QJsonArray getArray() const;
        qint64 getLong() const;
        QJsonObject getObject() const;
        QString getString() const;

    private:
        void nullCheck() const;

    private:
        QJsonValue var;
        QString key;

};

}

#endif // JSONVALIDATOR_H
