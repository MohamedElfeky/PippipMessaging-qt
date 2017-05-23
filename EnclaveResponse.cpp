#include "EnclaveResponse.h"
#include "SessionState.h"
#include <QJsonValue>
#include <QJsonDocument>
#include <QByteArray>
#include <coder/ByteArray.h>
#include <CryptoKitty-C/encoding/GCMCodec.h>

namespace Pippip {

EnclaveResponse::EnclaveResponse(const QJsonObject& j, SessionState *sess)
: encrypted(j),
  state(sess) {
}

EnclaveResponse::operator bool() {

    QJsonValue value = json["error"];
    if (value != QJsonValue::Null) {
        error = value.toString();
        return false;
    }

    QJsonValue dataValue = json["enclaveData"];
    if (dataValue.isNull()) {
        error = "Invalid server response";
        return false;
    }
    else {
        // QString hex string to coder byte array.
        coder::ByteArray enclaveData(dataValue.toString().toStdString(), true);
        // Decrypt and unload enclave data.
        CK::GCMCodec codec(enclaveData);
        codec.decrypt(state->enclaveKey, state->authData);
        coder::ByteArray jsonBytes;
        codec >> jsonBytes;
        // Coder byte array to QByteArray
        QByteArray jbytes(jsonBytes.toLiteralString().c_str(), jsonBytes.getLength());
        // Load json document.
        QJsonDocument doc = QJsonDocument::fromJson(jbytes);
        // Get json object.
        json = doc.object();
        // Get the request name
        QJsonValue name = json["requestName"];
        if (name.isNull() || !name.isString()) {
            error = "Invalid response from server";
            return false;
        }
        else {
            requestName = name.toString();
            return true;
        }
    }

}

QString EnclaveResponse::getValue(const QString &name) const {

    QJsonValue value = json[name];
    if (value.isNull() || !value.isString()) {
        return "";
    }
    else {
        return value.toString();
    }

}

} // namespace Pippip
