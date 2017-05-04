#include "EnclaveRequest.h"
#include "SessionState.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <coder/ByteArray.h>
#include <CryptoKitty-C/encoding/GCMCodec.h>

namespace Pippip {

static const QString ENCLAVE_REQUEST_URL = "https://pippip.io:2880/enclave/request/";

EnclaveRequest::EnclaveRequest(SessionState *sess)
: state(sess),
  request(new QJsonObject) {
}

EnclaveRequest::~EnclaveRequest() {
    delete request;
}

QString EnclaveRequest::getValue(const QString& name) const {

    QJsonValue value = (*request)[name];
    if (value != QJsonValue::Null) {
        return value.toString();
    }
    else {
        return "";
    }

}

void EnclaveRequest::setRequestType(const QString &type) {

    (*request)["requestType"] = type;

}

void EnclaveRequest::setValue(const QString& name, const QString& value) {

    (*request)[name] = value;

}

QJsonObject *EnclaveRequest::getJson() const {

    QJsonDocument doc(*request);
    QString jstr(doc.toJson());
    coder::ByteArray enclaveData(jstr.toStdString(), false);    // Literal data
    CK::GCMCodec codec;
    codec << enclaveData;
    codec.encrypt(state->enclaveKey, state->authData);
    QJsonObject *json = new QJsonObject;
    (*json)["enclaveData"] = QString(codec.toArray().toHexString().c_str());
    return json;

}

const QString& EnclaveRequest::getUrl() const {

    return ENCLAVE_REQUEST_URL;

}

} // namespace Pippip
