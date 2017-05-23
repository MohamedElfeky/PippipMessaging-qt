#include "ServerAuthorized.h"
#include "SessionState.h"
#include <CryptoKitty-C/encoding/RSACodec.h>
#include <QJsonObject>
#include <sstream>

namespace Pippip {

static const QString AUTHORIZED_URL = "https://pippip.io:2880/io.pippip.rest/Authorized";

ServerAuthorized::ServerAuthorized(SessionState *sess)
: state(sess) {
}

QJsonObject *ServerAuthorized::getJson() const {

    QJsonObject *json  = new QJsonObject;
    std::ostringstream sstr;
    (*json)["sessionId"] = static_cast<int>(state->sessionId);

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
