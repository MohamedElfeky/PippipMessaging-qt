#include "NewAccountFinal.h"
#include "SessionState.h"
#include <sstream>

namespace Pippip {

NewAccountFinal::NewAccountFinal(const QJsonObject& j, SessionState *sess)
: json(j),
  state(sess) {
}

NewAccountFinal::~NewAccountFinal() {
}

NewAccountFinal::operator bool() {

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        error = value.toString();
        return false;
    }

    QJsonValue keyValue = json["handlerKey"];
    QJsonValue authValue = json["authToken"];
    if (keyValue == QJsonValue::Null || authValue == QJsonValue::Null) {
        error = "Invalid server response";
        return false;
    }
    else {
        std::istringstream kstr(keyValue.toString().toStdString());
        kstr >> std::hex >> state->handlerKey;
        std::istringstream astr(authValue.toString().toStdString());
        astr >> std::hex >> state->authToken;
        return true;
    }

}

}
