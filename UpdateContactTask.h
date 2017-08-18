#ifndef UPDATECONTACTTASK_H
#define UPDATECONTACTTASK_H

#include "EnclaveRequestTask.h"

namespace Pippip {

struct Contact;

class UpdateContactTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        UpdateContactTask(SessionState *state, QObject *parent = 0);
        ~UpdateContactTask() {}

    public:
        const QString& getResult() const { return result; }
        void setContact(const Contact& contact);

    protected:
        bool requestComplete();

    private:
        QString result;

};

}
#endif // UPDATECONTACTTASK_H
