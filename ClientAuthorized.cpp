#include "ClientAuthorized.h"
#include "SessionState.h"
#include <coder/ByteArray.h>
#include <coder/Unsigned32.h>
#include <coder/Unsigned64.h>
#include <coder/OutOfRangeException.h>
#include <sstream>
#include <iostream>

namespace Pippip {

ClientAuthorized::ClientAuthorized(const QJsonObject& j, SessionState *sess)
: json(j),
  state(sess) {
}

ClientAuthorized::operator bool() {

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        error = value.toString();
        return false;
    }

    QJsonValue authValue = json["authToken"];
    if (authValue == QJsonValue::Null || !authValue.isDouble()) {
        error = "Invalid server response";
        return false;
    }
    else {
        state->authToken = authValue.toDouble();
        return true;
    }

}

} // namespace Pippip
