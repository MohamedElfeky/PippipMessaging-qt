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

    QJsonValue keyValue = json["handlerKey"];
    QJsonValue tokenValue = json["authToken"];
    if (keyValue == QJsonValue::Null || tokenValue == QJsonValue::Null) {
        error = "Invalid server response";
    }
    else {
        try {
            state->handlerKey = keyValue.toDouble();
            state->authToken = tokenValue.toDouble();
            return true;
        }
        catch (coder::OutOfRangeException& e) {
            std::ostringstream estr;
            estr << "Invalid server response - " << e.what();
            error = QString(estr.str().c_str());
        }
    }
    return false;

}

} // namespace Pippip
