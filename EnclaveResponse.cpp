#include "EnclaveResponse.h"
#include "SessionState.h"
#include "JsonValidator.h"
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QByteArray>
#include <QDebug>
#include <coder/ByteArray.h>
#include <CryptoKitty-C/encoding/GCMCodec.h>

namespace Pippip {

/**
 * @brief EnclaveResponse::EnclaveResponse
 * @param j
 * @param sess
 */
EnclaveResponse::EnclaveResponse(const QJsonObject& j, SessionState *sess)
: json(j),
  state(sess) {
}

/**
 * @brief EnclaveResponse::operator bool
 */
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
        QJsonValue rType = json["responseType"];
        if (!rType.isString()) {
            qDebug() << "Missing response type in enclave response";
            error = "Invalid response from server";
            return false;
        }
        else {
            responseType = rType.toString();
            return true;
        }
    }

}

/**
 * @brief EnclaveResponse::getResponseArray
 * @param name
 * @return
 */
QJsonArray EnclaveResponse::getResponseArray(const QString& name) const {

    return JsonValidator(json, name).getArray();

}

/**
 * @brief EnclaveResponse::getResponseLong
 * @param name
 * @return
 */
qint64 EnclaveResponse::getResponseLong(const QString &name) const {

    return JsonValidator(json, name).getLong();

}

/**
 * @brief EnclaveResponse::getResponseString
 * @param name
 * @return
 */
QString EnclaveResponse::getResponseString(const QString &name) const {

    return JsonValidator(json, name).getString();

}

/**
 * @brief EnclaveResponse::getResponseValue
 * @param name
 * @return
 */
QJsonValue EnclaveResponse::getResponseValue(const QString &name) const {

    return json[name];

}

} // namespace Pippip
