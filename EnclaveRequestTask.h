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

#include "RESTTask.h"

namespace Pippip {

class SessionState;
class EnclaveRequest;
class EnclaveResponse;

class EnclaveRequestTask : public RESTTask {
    Q_OBJECT

    protected:
        EnclaveRequestTask(SessionState *state, QObject *parent);

    public:
        ~EnclaveRequestTask();

    private:
        EnclaveRequestTask(const EnclaveRequestTask& other);
        EnclaveRequestTask& operator=(const EnclaveRequestTask& other);

    signals:
        void enclaveRequestFailed(const QString& error);
        void enclaveRequestTimedOut();

    public:
        const QString& getTaskName() const { return taskName; }

    protected:
        virtual void doRequest(int timeout);
        void restFailed(const QString& error);
        void restTimedOut();

    protected:
        EnclaveRequest *request;
        EnclaveResponse *response;
        SessionState *state;
        QString taskName;

};

}

#endif // EnclaveRequestTask_H
