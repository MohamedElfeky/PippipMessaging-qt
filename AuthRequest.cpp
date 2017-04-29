#include "AuthRequest.h"
#include "SessionState.h"
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <CryptoKitty-C/encoding/RSACodec.h>
#include <sstream>

namespace Pippip {

static const QString AUTH_URL = "https://pippip.io:2880/authenticate/";

AuthRequest::AuthRequest(SessionState *sess)
: state(sess) {
}

QJsonObject *AuthRequest::getJson() const {

    QJsonObject *json  = new QJsonObject;
    std::ostringstream sstr;
    sstr << std::hex << state->sessionId;
    (*json)["sessionId"] = QString(sstr.str().c_str());

    state->clientAuthRandom.setLength(32);
    CK::FortunaSecureRandom rnd;
    rnd.nextBytes(state->clientAuthRandom);
    (*json)["authRandom"] = QString(state->clientAuthRandom.toHexString().c_str());

    CK::RSACodec codec;
    codec << state->accountRandom;
    codec.encrypt(*state->serverPublicKey);
    (*json)["accountRandom"] = QString(codec.toArray().toHexString().c_str());
    (*json)["publicId"] = QString(state->publicId.toHexString().c_str());

    return json;

}

const QString& AuthRequest::getUrl() const {

    return AUTH_URL;

}

} // namespace Pippip
