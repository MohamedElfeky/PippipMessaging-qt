#include "AuthRequest.h"
#include "SessionState.h"
#include <CryptoKitty-C/random/FortunaSecureRandom.h>
#include <CryptoKitty-C/encoding/RSACodec.h>

namespace Pippip {

static const QString AUTH_URL = "https://pippip.io:2880/io.pippip.rest/AuthRequest";

AuthRequest::AuthRequest(SessionState *sess)
: state(sess) {
}

QJsonObject *AuthRequest::getJson() const {

    QJsonObject *json  = new QJsonObject;
    (*json)["sessionId"] = static_cast<int>(state->sessionId);

    state->clientAuthRandom.setLength(32);
    CK::FortunaSecureRandom rnd;
    rnd.nextBytes(state->clientAuthRandom);

    CK::RSACodec codec;
    codec << state->accountRandom << state->clientAuthRandom;
    codec.encrypt(*state->serverPublicKey);
    (*json)["encrypted"] = QString(codec.toArray().toHexString().c_str());
    (*json)["publicId"] = state->publicId;

    return json;

}

const QString& AuthRequest::getUrl() const {

    return AUTH_URL;

}

} // namespace Pippip
