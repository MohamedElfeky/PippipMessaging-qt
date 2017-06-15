#ifndef PIPPIP_NICKNAMEMANAGER_H
#define PIPPIP_NICKNAMEMANAGER_H

#include <QWidget>
#include <QString>
#include <vector>
//#include <map>

class QJsonObject;

class NicknamesDialog;

namespace Pippip {

class RESTHandler;
struct SessionState;

struct Nickname {
    QString nickname;
    QString policy;
    QString publicId;
};

typedef std::vector<Nickname> NicknameList;

class EnclaveResponse;

class NicknameManager : public QObject {
        Q_OBJECT

    public:
        NicknameManager(QWidget *parent, SessionState *state);
        ~NicknameManager() {}

    signals:
        void nicknameAdded(QString name, QString policy);
        void nicknameDeleted(QString name);
        void nicknamesLoaded();
        void policyUpdated(QString name, QString policy);

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
        QString getPolicy(const QString& nick) const;
        const NicknameList& getNicknames() const { return nicknames; }
        void loadNicknames();
        void manageNicknames();
        void updatePolicy(const Nickname& nickname);

    private:
        bool getNickname(const EnclaveResponse& resp, Nickname& nickname);
        bool loadNicknames(const QJsonObject& json);

    private:
        bool loaded;
        bool requestComplete;
        bool timedOut;
        SessionState *state;
        NicknamesDialog *dialog;

        NicknameList nicknames;

};

} // namespace Pippip

#endif // PIPPIP_NICKNAMEMANAGER_H
