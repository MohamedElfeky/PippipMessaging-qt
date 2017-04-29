#include "NewAccountFinish.h"
#include "SessionState.h"
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <CryptoKitty-C/encoding/RSACodec.h>
#include <sstream>

namespace Pippip {

static const QString FINISH_URL = "https://pippip.io:2880/accountfinish/";

NewAccountFinish::NewAccountFinish(SessionState *sess)
: state(sess) {
}

QJsonObject *NewAccountFinish::getJson() const {

    QJsonObject *json  = new QJsonObject;
    std::ostringstream sstr;
    sstr << std::hex << state->sessionId;
    (*json)["sessionId"] = QString(sstr.str().c_str());

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
