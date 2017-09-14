#include "JsonValidator.h"
#include "ValidationException.h"
#include <QJsonArray>
#include <sstream>

namespace Pippip {

/**
 * @brief JsonValidator::JsonValidator
 * @param obj
 * @param k
 */
JsonValidator::JsonValidator(const QJsonObject& obj, const QString& k)
: key(k) {

    var = obj[key];

}

/**
 * @brief JsonValidator::getArray
 * @return
 */
QJsonArray JsonValidator::getArray() const {

    nullCheck();
    if (var.isArray()) {
        return var.toArray();
    }
    else {
        std::ostringstream estr;
        estr << "'" << key.constData() << "' is not an array";
        throw ValidationException(estr.str());
    }

}

/**
 * @brief JsonValidator::getLong
 * @return
 */
qint64 JsonValidator::getLong() const {

    nullCheck();
    if (var.isDouble()) {
        return var.toDouble();
    }
    else {
        std::ostringstream estr;
        estr << "'" << key.constData() << "' is not a long integer";
        throw ValidationException(estr.str());
    }

}

/**
 * @brief JsonValidator::getObject
 * @return
 */
QJsonObject JsonValidator::getObject() const {

    nullCheck();
    if (var.isObject()) {
        return var.toObject();
    }
    else {
        std::ostringstream estr;
        estr << "'" << key.constData() << "' is not an object";
        throw ValidationException(estr.str());
    }

}

/**
 * @brief JsonValidator::getString
 * @return
 */
QString JsonValidator::getString() const {

    nullCheck();
    if (var.isString()) {
        return var.toString();
    }
    else {
        std::ostringstream estr;
        estr << "'" << key.constData() << "' is not a string";
        throw ValidationException(estr.str());
    }

}

/**
 * @brief JsonValidator::nullCheck
 */
void JsonValidator::nullCheck() const {

    if (var.isNull()) {
        std::ostringstream estr;
        estr << "'" << key.constData() << "' is null";
        throw ValidationException(estr.str());
    }

}

}
