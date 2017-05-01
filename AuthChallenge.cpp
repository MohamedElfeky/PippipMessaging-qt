#include "AuthChallenge.h"
#include "SessionState.h"
#include <CryptoKitty-C/digest/SHA256.h>
#include <CryptoKitty-C/mac/HMAC.h>
#include <CryptoKitty-C/cipher/PKCS1rsassa.h>
#include <sstream>

namespace Pippip {

static const QString CHALLENGE_URL = "https://pippip.io:2880/authchallenge/";

AuthChallenge::AuthChallenge(SessionState *sess)
: state(sess){
}

AuthChallenge::AuthChallenge(const QJsonObject& j, SessionState *sess)
: json(j),
  state(sess) {
}

void AuthChallenge::generateClientHmac() const {

    s2k();

    CK::HMAC mac(new CK::SHA256);
    mac.setKey(hmacKey);
    coder::ByteArray message(state->serverAuthRandom);
    message.append("secomm server");
    mac.setMessage(message);
    hmac = mac.getHMAC();
    CK::PKCS1rsassa rsassa(new CK::SHA256);
    signature = rsassa.sign(*state->userPrivateKey, hmac);

}

QJsonObject *AuthChallenge::getJson() const {

    QJsonObject *json  = new QJsonObject;
    std::ostringstream sstr;
    sstr << std::hex << state->sessionId;
    (*json)["sessionId"] = QString(sstr.str().c_str());

    generateClientHmac();
    (*json)["hmac"] = QString(hmac.toHexString().c_str());
    (*json)["signature"] = QString(signature.toHexString().c_str());

    return json;

}

const QString& AuthChallenge::getUrl() const {

    return CHALLENGE_URL;

}

AuthChallenge::operator bool() {

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        error = value.toString();
        return false;
    }

    QJsonValue hmacValue = json["hmac"];
    QJsonValue sigValue = json["signature"];
    if (hmacValue == QJsonValue::Null || sigValue == QJsonValue::Null) {
        error = "Invalid server response";
        return false;
    }
    else {
        coder::ByteArray hmac(hmacValue.toString().toStdString(), true);
        coder::ByteArray sig(sigValue.toString().toStdString(), true);
        CK::PKCS1rsassa rsassa(new CK::SHA256);
        if (rsassa.verify(*state->serverPublicKey, hmac, sig)) {
            s2k();
            CK::HMAC mac(new CK::SHA256);
            mac.setKey(hmacKey);
            coder::ByteArray message(state->clientAuthRandom);
            message.append("secomm client");
            mac.setMessage(message);
            if (mac.authenticate(hmac)) {
                return true;
            }
            else {
                error = "Authentication challenge failed";
                return false;
            }
        }
        else {
            error = "Signature verification failed";
            return false;
        }
    }

}

void AuthChallenge::s2k() const {

    uint8_t c = state->genpass[state->genpass.getLength()-1];
    int count =  c & 0x0f;
    if (count == 0) {
        count = 0x0c;
    }
    count = count << 16;

    coder::ByteArray message(state->genpass);
    message.append("@secomm.org");
    message.append(state->accountRandom);
    CK::SHA256 digest;
    coder::ByteArray hash(digest.digest(message));
    count -= 32;
    while (count > 0) {
        coder::ByteArray ctx(message);
        ctx.append(hash);
        hash = digest.digest(ctx);
        count = count - 32 - message.getLength();
    }
    hmacKey = hash;

}

} // namespace Pippip
