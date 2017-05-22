#include "NewAccountFinish.h"
#include "SessionState.h"
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <CryptoKitty-C/encoding/RSACodec.h>
#include <sstream>

namespace Pippip {

static const QString FINISH_URL = "https://pippip.io:2880/io.pippip.rest/NewAccountFinish";

NewAccountFinish::NewAccountFinish(SessionState *sess)
: state(sess) {
}

QJsonObject *NewAccountFinish::getJson() const {

    QJsonObject *json  = new QJsonObject;
    (*json)["sessionId"] = static_cast<int>(state->sessionId);

    CK::RSACodec codec;
    codec << state->genpass << state->enclaveKey;
    codec.encrypt(*state->serverPublicKey);
    QString encrypted(codec.toArray().toHexString().c_str());
    (*json)["encrypted"] = encrypted;

    return json;

}

const QString& NewAccountFinish::getUrl() const {

    return FINISH_URL;

}

}
