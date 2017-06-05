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

    QJsonValue authValue = json["authToken"];
    if (authValue == QJsonValue::Null && !authValue.isDouble()) {
        error = "Invalid server response";
        return false;
    }
    else {
        state->authToken = authValue.toDouble();
        return true;
    }

}

}
