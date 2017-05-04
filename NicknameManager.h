#ifndef PIPPIP_NICKNAMEMANAGER_H
#define PIPPIP_NICKNAMEMANAGER_H

#include <QWidget>
#include <QString>
#include <map>

class QJsonObject;

namespace Pippip {

class RESTHandler;
struct SessionState;

class NicknameManager : public QWidget {
        Q_OBJECT

    public:
        NicknameManager(QWidget *parent, SessionState *state);
        ~NicknameManager() {}

    protected slots:
        void loadComplete(RESTHandler*);
        void loadTimedOut();

    public:
        void manageNicknames();

    private:
        bool loadNicknames(const QJsonObject& json);

    private:
        bool loadedComplete;
        bool timedOut;
        SessionState *state;

        typedef std::map<QString, int> PolicyMap;
        typedef PolicyMap::const_iterator PolicyConstIter;
        PolicyMap policyMap;

};

} // namespace Pippip

#endif // PIPPIP_NICKNAMEMANAGER_H
