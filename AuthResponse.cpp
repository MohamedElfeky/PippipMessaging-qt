#include "AuthResponse.h"
#include "SessionState.h"
#include <coder/ByteArray.h>
#include <CryptoKitty-C/encoding/RSACodec.h>

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

    QJsonValue rndValue = json["encrypted"];
    if (rndValue == QJsonValue::Null) {
        error = "Invalid server response";
        return false;
    }
    else {
        CK::RSACodec codec(coder::ByteArray(rndValue.toString().toStdString(), true));
        codec.decrypt(*state->userPrivateKey);
        codec >> state->serverAuthRandom;
        return true;
    }

}

} // namespace Pippip
