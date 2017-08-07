#include "NicknameManager.h"
#include "EnclaveRequest.h"
#include "SessionState.h"
#include "RESTHandler.h"
#include "EnclaveResponse.h"
#include "EnclaveException.h"
#include "Constants.h"
#include "Nicknames.h"
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <assert.h>
#include <iostream>

namespace Pippip {

NicknameManager::NicknameManager(QWidget *parent, SessionState *sess)
: QObject(parent),
  loaded(false),
  requestCompleted(false),
  timedOut(false),
  state(sess),
  nicknames(new Nicknames){
}

NicknameManager::~NicknameManager() {

    delete nicknames;

}

void NicknameManager::addNickname(const Nickname &nick) {

    inProgress = nicknameAdd;
    EnclaveRequest req(state);
    req.setRequestType("addNickname");
    req.setValue("nickname", nick.entity.nickname);
    req.setValue("policy", nick.policy);
    req.setValue("whitelist", encodeWhitelist(nick));
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

bool NicknameManager::decodeEntity(const QJsonObject &obj, Entity &entity) const {

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

bool NicknameManager::decodeNickname(const QJsonObject &obj, Nickname &nickname) const {

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

bool NicknameManager::decodeRSAKeys(const QJsonObject &obj, RSAKeys &rsaKeys) const {

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

void NicknameManager::deleteNickname(const QString& nickname) {

    inProgress = nicknameDelete;
    EnclaveRequest req(state);
    req.setRequestType("deleteNickname");
    req.setValue("nickname", nickname);
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

QJsonObject NicknameManager::encodeNickname(const Nickname &nickname) {

    QJsonObject nickObj;
    QJsonObject entityObj;
    entityObj["nickname"] = nickname.entity.nickname;
    nickObj["entity"] = entityObj;
    nickObj["policy"] = nickname.policy;
    nickObj["whitelist"] = encodeWhitelist(nickname);

    return nickObj;

}

QJsonArray NicknameManager::encodeWhitelist(const Nickname &nickname) {

    QJsonArray whitelist;
    for (auto entity : nickname.whitelist) {
        QJsonObject wlEntityObj;
        wlEntityObj["nickname"] = entity.nickname;
        wlEntityObj["publicId"] = entity.publicId;
        whitelist.append(wlEntityObj);
    }
    return whitelist;

}

void NicknameManager::loadComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            emit requestFailed("Fetch Nicknames", handler->getError());
        }
        else if (response) {
            if (response.getResponseType() == "getNicknames" && loadNicknames(response.getJson())) {
                loaded = true;
                emit nicknamesLoaded();
            }
            else {
                emit requestFailed("Fetch Nicknames", "Invalid server response");
            }
        }
        else {
            emit requestFailed("Fetch Nicknames", response.getError());
        }
    }

}

void NicknameManager::loadNicknames() {

    if (!loaded) {
        nicknames->clear();
        EnclaveRequest req(state);
        req.setRequestType("getNicknames");
        RESTHandler *handler = new RESTHandler(this);
        connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(loadComplete(RESTHandler*)));
        connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(loadComplete(RESTHandler*)));
        timedOut = false;
        QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
        handler->doPost(req);
    }
    else {
        emit nicknamesLoaded();
    }

}

bool NicknameManager::loadNicknames(const QJsonObject &json) {

    QJsonValue nickValue = json["nicknames"];
    if (!nickValue.isArray()) {
        qDebug() << "Missing nicknames array in get nicknames response";
        return false;
    }

    QJsonArray nicknamesJson = nickValue.toArray();
    for (const QJsonValue& value : nicknamesJson) {
        Nickname nickname;
        if (!value.isObject() || !decodeNickname(value.toObject(), nickname)) {
            return false;
        }
        nicknames->append(nickname);
    }
    return true;

}

/*
 * Validate the response here. Status handling is in the nickname and whitelist handlers.
 */
void NicknameManager::requestComplete(RESTHandler *handler) {

    if (!timedOut) {
        requestCompleted = true;

        EnclaveResponse response(handler->getResponse(), state);
        if (!handler->successful()) {
            requestError(handler->getError());
        }
        else if (response) {
            loaded = false;
            if (response.getValue("status").isString()) {
                requestValid(response);
            }
            else {
                requestError("Invalid server response");
            }
        }
        else {
            requestError(response.getError());
        }
    }

}

void NicknameManager::requestError(const QString &error) {

    switch (inProgress) {
        case nicknameAdd:
            emit requestFailed(Constants::ADD_NICKNAME, error);
            emit taskFailed(Constants::ADD_NICKNAME);
            break;
        case nicknameDelete:
            emit requestFailed(Constants::DELETE_NICKNAME, error);
            emit taskFailed(Constants::DELETE_NICKNAME);
            break;
        case policyModify:
            emit requestFailed(Constants::UPDATE_CONTACT_POLICY, error);
            emit taskFailed(Constants::UPDATE_CONTACT_POLICY);
            break;
        case whitelistModify:
            emit requestFailed(Constants::UPDATE_WHITELIST, error);
            emit taskFailed(Constants::UPDATE_WHITELIST);
            break;
    }

}

void NicknameManager::requestValid(const EnclaveResponse &response) {

    QString status = response.getValue("status").toString();
    switch (inProgress) {
        case nicknameAdd:
            emit addCompleted(status);
            break;
        case nicknameDelete:
            emit deleteCompleted(status);
            break;
        case policyModify:
            emit policyUpdated(status);
            break;
        case whitelistModify:
            loaded = false;
            emit whitelistUpdated(status);
            break;
    }

}

void NicknameManager::requestTimedOut() {

    if (!requestCompleted) {
        timedOut = true;
        emit requestFailed("Nickname Request", "Request timed out");
    }

}

void NicknameManager::updatePolicy(const QString& nickname, const QString& policy) {

    inProgress = policyModify;
    EnclaveRequest req(state);
    req.setRequestType("updateNickname");
    req.setValue("nickname", nickname);
    req.setValue("policy", policy);
    req.setValue("type", QString("policy"));
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

void NicknameManager::updateWhitelist(const QString& nickname, const Entity& whitelistEntity,
                                        const QString& action) {

    inProgress = whitelistModify;
    EnclaveRequest req(state);
    req.setRequestType("updateNickname");
    req.setValue("nickname", nickname);
    req.setValue("whitelistNickname", whitelistEntity.nickname);
    req.setValue("whitelistPublicId", whitelistEntity.publicId);
    req.setValue("action", action);
    req.setValue("type", QString("whitelist"));
    RESTHandler *handler = new RESTHandler(this);
    connect(handler, SIGNAL(requestComplete(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    connect(handler, SIGNAL(requestFailed(RESTHandler*)), this, SLOT(requestComplete(RESTHandler*)));
    timedOut = false;
    QTimer::singleShot(10000, this, SLOT(requestTimedOut()));
    handler->doPost(req);

}

} // namespace Pippip
