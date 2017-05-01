#include "AuthResponse.h"
#include "SessionState.h"
#include <coder/ByteArray.h>

namespace Pippip {

AuthResponse::AuthResponse(const QJsonObject& j, SessionState *sess)
: json(j),
  state(sess) {
}

AuthResponse::~AuthResponse() {
}

AuthResponse::operator bool() {

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        error = value.toString();
        return false;
    }

    QJsonValue rndValue = json["authRandom"];
    if (rndValue == QJsonValue::Null) {
        error = "Invalid server response";
        return false;
    }
    else {
        state->serverAuthRandom =
                coder::ByteArray(rndValue.toString().toStdString(), true);
        return true;
    }

}

} // namespace Pippip
