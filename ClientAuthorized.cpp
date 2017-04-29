#include "ClientAuthorized.h"
#include "SessionState.h"
#include <coder/ByteArray.h>
#include <coder/Unsigned32.h>
#include <coder/Unsigned64.h>

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

    QJsonValue keyValue = json["handlerKey"];
    QJsonValue tokenValue = json["authToken"];
    if (keyValue == QJsonValue::Null || tokenValue == QJsonValue::Null) {
        error = "Invalid server response";
        return false;
    }
    else {
        coder::ByteArray handlerKey(keyValue.toString().toStdString(), true);
        coder::Unsigned32 u32(handlerKey, coder::bigendian);
        state->handlerKey = u32.getValue();
        coder::ByteArray authToken(tokenValue.toString().toStdString(), true);
        coder::Unsigned64 u64(authToken, coder::bigendian);
        state->authToken = u64.getValue();
        return true;
    }

}

} // namespace Pippip
