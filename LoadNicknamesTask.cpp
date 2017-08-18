#include "LoadNicknamesTask.h"
#include "Constants.h"
#include "EnclaveResponse.h"
#include "SessionState.h"
#include "Nicknames.h"
#include <QJsonObject>
#include <QJsonArray>

namespace Pippip {

LoadNicknamesTask::LoadNicknamesTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::LOAD_NICKNAMES_TASK;

}

bool LoadNicknamesTask::decodeEntity(const QJsonObject &obj, Entity &entity) const {

    QJsonValue nicknameValue = obj["nickname"];
    if (!nicknameValue.isString()) {
        return false;
    }
    entity.nickname = nicknameValue.toString();

    QJsonValue publicIdValue = obj["publicId"];
    if (!publicIdValue.isString()) {
        return false;
    }
    entity.publicId = publicIdValue.toString();

    return true;

}

bool LoadNicknamesTask::decodeNickname(const QJsonObject &obj, Nickname &nickname) const {

    QJsonValue entityValue = obj["entity"];
    if (!entityValue.isObject()) {
        return false;
    }
    if (!decodeEntity(entityValue.toObject(), nickname.entity)) {
        return false;
    }

    QJsonValue rsaKeysValue = obj["rsaKeys"];
    if (!rsaKeysValue.isObject()) {
        return false;
    }
    if (!decodeRSAKeys(rsaKeysValue.toObject(), nickname.rsaKeys)) {
        return false;
    }

    QJsonValue policyValue = obj["policy"];
    if (!policyValue.isString()) {
        return false;
    }
    nickname.policy = policyValue.toString();

    QJsonValue whitelistValue = obj["whitelist"];
    if (!whitelistValue.isArray()) {
        return false;
    }
    QJsonArray whitelist = whitelistValue.toArray();
    for (const QJsonValue& wlEntityValue : whitelist) {
        Entity wlEntity;
        if (!wlEntityValue.isObject() || !decodeEntity(wlEntityValue.toObject(), wlEntity)) {
            return false;
        }
        nickname.whitelist.push_back(wlEntity);
    }

    return true;

}

bool LoadNicknamesTask::decodeRSAKeys(const QJsonObject &obj, RSAKeys &rsaKeys) const {

    QJsonValue encryptionRSAValue = obj["encryptionRSA"];
    if (!encryptionRSAValue.isString()) {
        return false;
    }
    rsaKeys.encryptionRSA = encryptionRSAValue.toString();

    QJsonValue signingRSAValue = obj["signingRSA"];
    if (!signingRSAValue.isString()) {
        return false;
    }
    rsaKeys.signingRSA = signingRSAValue.toString();

    return true;

}

bool LoadNicknamesTask::requestComplete() {

    state->nicknames->clear();
    QJsonValue nickValue = response->getValue("nicknames");
    if (!nickValue.isArray()) {
        error = "Invalid server response";
        return false;
    }

    QJsonArray nicknamesJson = nickValue.toArray();
    for (const QJsonValue& value : nicknamesJson) {
        Nickname nickname;
        if (!value.isObject() || !decodeNickname(value.toObject(), nickname)) {
            error = "Invalid server response";
            return false;
        }
        state->nicknames->append(nickname);
    }
    return true;

}

}
