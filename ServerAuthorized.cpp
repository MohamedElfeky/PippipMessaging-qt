#include "ServerAuthorized.h"
#include "SessionState.h"
#include <CryptoKitty-C/encoding/RSACodec.h>
#include <QJsonObject>
#include <sstream>

namespace Pippip {

static const QString AUTHORIZED_URL = "https://pippip.io:2880/authorized/";

ServerAuthorized::ServerAuthorized(SessionState *sess)
: state(sess) {
}

QJsonObject *ServerAuthorized::getJson() const {

    QJsonObject *json  = new QJsonObject;
    std::ostringstream sstr;
    sstr << std::hex << state->sessionId;
    (*json)["sessionId"] = QString(sstr.str().c_str());

    CK::RSACodec codec;
    codec << state->enclaveKey;
    codec.encrypt(*state->serverPublicKey);
    (*json)["encrypted"] = QString(codec.toArray().toHexString().c_str());

    return json;

}

const QString& ServerAuthorized::getUrl() const {

    return AUTHORIZED_URL;

}

} // namespace Pippip
