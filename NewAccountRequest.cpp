#include "NewAccountRequest.h"
#include "SessionState.h"
#include <CryptoKitty-C/encoding/PEMCodec.h>
#include <sstream>

namespace Pippip {

static const QString REQUEST_URL = "https://pippip.io:2880/newaccount/";

NewAccountRequest::NewAccountRequest(SessionState *sess)
: state(sess) {
}

NewAccountRequest::~NewAccountRequest() {
}

QJsonObject *NewAccountRequest::getJson() const {

    QJsonObject *json  = new QJsonObject;
    std::ostringstream sstr;
    sstr << std::hex << state->sessionId;
    (*json)["sessionId"] = QString(sstr.str().c_str());
    (*json)["publicId"] = QString(state->publicId.toHexString().c_str());
    CK::PEMCodec codec(true);   // X.509 keys
    std::ostringstream kstr;
    codec.encode(kstr, *state->userPublicKey);  // Handle exceptions here?
    (*json)["userPublicKey"] = QString(kstr.str().c_str());

    return json;

}

const QString& NewAccountRequest::getUrl() const {

    return REQUEST_URL;

}

}
