#include "EnclaveRequest.h"
#include "SessionState.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <coder/ByteArray.h>
#include <CryptoKitty-C/encoding/GCMCodec.h>

namespace Pippip {

static const QString ENCLAVE_REQUEST_URL = "https://pippip.io:2880/io.pippip.rest/EnclaveRequest";

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

void EnclaveRequest::setValue(const QString &name, quint64 value) {

    // Derp
    (*request)[name] = static_cast<qint64>(value);

}

void EnclaveRequest::setValue(const QString& name, const QString& value) {

    (*request)[name] = value;

}

void EnclaveRequest::setValue(const QString& name, const QJsonObject& value) {

    (*request)[name] = value;

}

void EnclaveRequest::setValue(const QString& name, const QJsonArray& value) {

    (*request)[name] = value;

}

void EnclaveRequest::setValue(const QString &name, const QByteArray &value) {

    (*request)[name] = QString(value.toHex());

}

QJsonObject *EnclaveRequest::getJson() const {

    QJsonObject *json = new QJsonObject;
    (*json)["sessionId"] = static_cast<int>(state->sessionId);

    // The enclave data is an encrypted JSON string.
    QJsonDocument doc(*request);
    QString jstr(doc.toJson());
    coder::ByteArray enclaveData(jstr.toStdString(), false);    // Literal data
    CK::GCMCodec codec;
    codec << enclaveData;
    codec.encrypt(state->enclaveKey, state->authData);

    (*json)["authToken"] = static_cast<qint64>(state->authToken);
    (*json)["enclaveData"] = QString(codec.toArray().toHexString().c_str());
    return json;

}

const QString& EnclaveRequest::getUrl() const {

    return ENCLAVE_REQUEST_URL;

}

} // namespace Pippip
