#include "LoadNicknamesTask.h"
#include "Constants.h"
#include "EnclaveResponse.h"
#include "JsonValidator.h"
#include "StringCodec.h"
#include "ValidationException.h"
#include <QJsonObject>
#include <QJsonArray>
#include <sstream>

namespace Pippip {

/**
 * @brief LoadNicknamesTask::LoadNicknamesTask
 * @param state
 * @param parent
 */
LoadNicknamesTask::LoadNicknamesTask(SessionState *state, QObject *parent)
: EnclaveRequestTask(state, parent) {

    taskName = Constants::LOAD_NICKNAMES_TASK;

}

/**
 * @brief LoadNicknamesTask::decodeEntity
 * @param obj
 * @param entity
 */
void LoadNicknamesTask::decodeEntity(const QJsonObject &obj, Entity &entity) const {

    entity.nickname = JsonValidator(obj, "nickname").getString();
    entity.publicId = JsonValidator(obj, "publicId").getString();

}

/**
 * @brief LoadNicknamesTask::decodeNickname
 * @param obj
 * @param nickname
 */
void LoadNicknamesTask::decodeNickname(const QJsonObject &obj, Nickname &nickname) const {

    QJsonObject entityObj = JsonValidator(obj, "entity").getObject();
    decodeEntity(entityObj, nickname.entity);

    QJsonObject rsaKeysObj = JsonValidator(obj, "rsaKeys").getObject();
    decodeRSAKeys(rsaKeysObj, nickname.rsaKeys);

    nickname.policy = JsonValidator(obj, "policy").getString();

    QJsonArray whitelist = JsonValidator(obj, "whitelist").getArray();
    for (const QJsonValue& wlEntityValue : whitelist) {
        Entity wlEntity;
        if (wlEntityValue.isObject()) {
            decodeEntity(wlEntityValue.toObject(), wlEntity);
        }
        else {
            throw ValidationException("Whitelist entry not an object");
        }
        nickname.whitelist.push_back(wlEntity);
    }

}

/**
 * @brief LoadNicknamesTask::decodeRSAKeys
 * @param obj
 * @param rsaKeys
 */
void LoadNicknamesTask::decodeRSAKeys(const QJsonObject &obj, RSAKeys &rsaKeys) const {

    rsaKeys.encryptionRSA = JsonValidator(obj, "encryptionRSA").getString();
    rsaKeys.signingRSA = JsonValidator(obj, "signingRSA").getString();

}

/**
 * @brief LoadNicknamesTask::loadNicknames
 */
void LoadNicknamesTask::loadNicknames() {

    nicknameList.clear();
    doRequest(10);

}

/**
 * @brief LoadNicknamesTask::restComplete
 * @param resp
 */
void LoadNicknamesTask::restComplete(const QJsonObject& resp) {

    try {
        response = new EnclaveResponse(resp, state);
        if (*response) {
            QJsonArray nickArray = response->getResponseArray("nicknames");
            for (const QJsonValue& value : nickArray) {
                Nickname nickname;
                if (value.isObject()) {
                    decodeNickname(value.toObject(), nickname);
                    nicknameList.push_back(nickname);
                }
                // TODO Handle type error.
            }
            emit loadNicknamesComplete();
        }
        else {
            emit loadNicknamesFailed(response->getError());
        }
    }
    catch (ValidationException& e) {
        std::ostringstream estr;
        estr << "Invalid server response - " << e.what();
        emit loadNicknamesFailed(StringCodec(estr.str()));
    }

}

/**
 * @brief LoadNicknamesTask::restFailed
 * @param error
 */
void LoadNicknamesTask::restFailed(const QString &error) {

    emit loadNicknamesFailed(error);

}

/**
 * @brief LoadNicknamesTask::restTimedOut
 */
void LoadNicknamesTask::restTimedOut() {

    emit loadNicknamesFailed("Load nicknames request timed out");

}

}
