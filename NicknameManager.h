#ifndef PIPPIP_NICKNAMEMANAGER_H
#define PIPPIP_NICKNAMEMANAGER_H

#include "Nickname.h"
#include <QWidget>
#include <QString>
#include <QJsonObject>
#include <vector>

class NicknamesDialog;

namespace Pippip {

class RESTHandler;
class SessionState;

class EnclaveResponse;

class NicknameManager : public QObject {
        Q_OBJECT

    public:
        NicknameManager(QWidget *parent, SessionState *state);
        ~NicknameManager() {}

    signals:
        void nicknameAdded();
        void nicknameDeleted(const QString& name);
        void nicknameExists();
        void nicknamesLoaded();
        void nicknameUpdated(const Pippip::Nickname&);
        void requestFailed(const QString& reqName, const QString& error);

    protected slots:
        void addComplete(RESTHandler*);
        void delComplete(RESTHandler*);
        void loadComplete(RESTHandler*);
        void requestTimedOut();
        void updateComplete(RESTHandler*);

    public:
        void addNickname(const Nickname& nick);
        void deleteNickname(const QString& nick);
        const Nickname& getNickname(const QString& nick) const;
        const NicknameList& getNicknames() const { return nicknames; }
        //bool isLoaded() const { return loaded; }
        void loadNicknames();
        void manageNicknames();
        void updateNickname(const Nickname& nickname);

    private:
        void deleteFromNicknames(const QString& nickname);
        bool decodeEntity(const QJsonObject& obj, Entity& entity) const;
        bool decodeNickname(const QJsonObject& obj, Nickname& nickname) const;
        bool decodeRSAKeys(const QJsonObject& obj, RSAKeys& rsaKey) const;
        QJsonObject encodeNickname(const Nickname& nickname);
        QJsonArray encodeWhitelist(const Nickname& nickname);
        bool getNickname(const EnclaveResponse& resp, Nickname& nickname);
        bool loadNicknames(const QJsonObject& json);
        void updatePolicy(const Nickname& nickname);

    private:
        bool loaded;
        bool requestComplete;
        bool timedOut;
        SessionState *state;
        NicknameList nicknames;

};

} // namespace Pippip

#endif // PIPPIP_NICKNAMEMANAGER_H
