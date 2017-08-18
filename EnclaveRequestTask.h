/*
 * EnclaveRequestTask.h
 * Copyright (C) 2017 Steve Brenneis <steve.brenneis@secomm.org>
 *
 * PippipMessaging is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PippipMessaging is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EnclaveRequestTask_H
#define EnclaveRequestTask_H

#include "EnclaveRequest.h"
#include <QObject>

namespace Pippip {

class SessionState;
class RESTHandler;
class EnclaveResponse;

class EnclaveRequestTask : public QObject {
    Q_OBJECT

    protected:
        EnclaveRequestTask(SessionState *state, QObject *parent);

    public:
        ~EnclaveRequestTask();

    private:
        EnclaveRequestTask(const EnclaveRequestTask& other);
        EnclaveRequestTask& operator=(const EnclaveRequestTask& other);

    signals:
        void requestComplete(Pippip::EnclaveRequestTask *task);
        void requestFailed(Pippip::EnclaveRequestTask *task);

    public slots:
        void requestComplete(RESTHandler *handler);
        void requestTimedOut();

    public:
        virtual void doRequest();
        const QString& getError() const { return error; }
        const QString& getTaskName() const { return taskName; }

    protected:
        virtual bool requestComplete()=0;

    protected:
        EnclaveRequest request;
        EnclaveResponse *response;
        SessionState *state;
        QString taskName;
        QString error;

    private:
        bool requestCompleted;
        bool timedOut;

};

}

#endif // EnclaveRequestTask_H
