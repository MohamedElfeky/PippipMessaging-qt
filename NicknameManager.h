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
struct SessionState;

class EnclaveResponse;

class NicknameManager : public QObject {
        Q_OBJECT

    public:
        NicknameManager(QWidget *parent, SessionState *state);
        ~NicknameManager() {}

    signals:
        void nicknameAdded(QString name, QString policy);
        void nicknameDeleted(QString name);
        void nicknamesLoaded(const Pippip::NicknameList&);
        void nicknameUpdated(const Pippip::Nickname&);

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
        QJsonObject encodeNickname(const Nickname& nickname);
        bool getNickname(const EnclaveResponse& resp, Nickname& nickname);
        bool loadNicknames(const QJsonObject& json);

    private:
        bool loaded;
        bool requestComplete;
        bool timedOut;
        SessionState *state;
        NicknameList nicknames;

};

} // namespace Pippip

#endif // PIPPIP_NICKNAMEMANAGER_H
