#include "NewAccountResponse.h"
#include "SessionState.h"
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <CryptoKitty-C/encoding/RSACodec.h>

namespace Pippip {

NewAccountResponse::NewAccountResponse(const QJsonObject& j, SessionState *sess)
: json(j),
  state(sess) {
}

NewAccountResponse::~NewAccountResponse() {
}

NewAccountResponse::operator bool() {

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        error = value.toString();
        return false;
    }

    QJsonValue keyValue = json["serverPublicKey"];
    QJsonValue encValue = json["encrypted"];
    if (keyValue == QJsonValue::Null || encValue == QJsonValue::Null) {
        error = "Invalid server response";
        return false;
    }
    else {
        CK::PEMCodec pemcodec;
        state->serverPublicKey = pemcodec.decodePublicKey(keyValue.toString().toStdString());
        CK::RSACodec rsacodec(coder::ByteArray(encValue.toString().toStdString(), true));
        rsacodec.decrypt(*state->userPrivateKey);
        rsacodec >> state->accountRandom;
        return true;
    }

}

}
