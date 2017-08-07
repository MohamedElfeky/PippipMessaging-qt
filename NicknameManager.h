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
class Nicknames;
class EnclaveResponse;

class NicknameManager : public QObject {
        Q_OBJECT

    public:
        NicknameManager(QWidget *parent, SessionState *state);
        ~NicknameManager();

    signals:
        void addCompleted(const QString& status);
        void deleteCompleted(const QString& status);
        void nicknameExists();
        void nicknamesLoaded();
        void policyUpdated(const QString& status);
        void requestFailed(const QString& reqName, const QString& error);
        void taskFailed(const QString& taskName);
        void whitelistUpdated(const QString& status);

    protected slots:
        void loadComplete(RESTHandler*);
        void requestComplete(RESTHandler*);
        void requestTimedOut();

    public:
        void addNickname(const Nickname& nick);
        void deleteNickname(const QString& nick);
        Nicknames *getNicknames() const { return nicknames; }
        void loadNicknames();
        void updatePolicy(const QString& nickname, const QString& policy);
        void updateWhitelist(const QString& nickname, const Entity& whitelistEntity,
                             const QString& action);

    private:
        bool decodeEntity(const QJsonObject& obj, Entity& entity) const;
        bool decodeNickname(const QJsonObject& obj, Nickname& nickname) const;
        bool decodeRSAKeys(const QJsonObject& obj, RSAKeys& rsaKey) const;
        QJsonObject encodeNickname(const Nickname& nickname);
        QJsonArray encodeWhitelist(const Nickname& nickname);
        bool loadNicknames(const QJsonObject& json);
        void requestError(const QString& error);
        void requestValid(const EnclaveResponse& response);

    private:
        bool loaded;
        bool requestCompleted;
        bool timedOut;
        enum Task { nicknameAdd, nicknameDelete, policyModify, whitelistModify };
        Task inProgress;
        SessionState *state;
        Nicknames *nicknames;

};

} // namespace Pippip

#endif // PIPPIP_NICKNAMEMANAGER_H
