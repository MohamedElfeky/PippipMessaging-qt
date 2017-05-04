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
        return true;
    }

}

} // namespace Pippip
