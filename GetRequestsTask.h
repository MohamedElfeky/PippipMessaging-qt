/*
 * GetRequestsTask.h
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

#ifndef GETREQUESTSTASK_H
#define GETREQUESTSTASK_H

#include "EnclaveRequestTask.h"
#include "ContactRequest.h"

namespace Pippip {

class GetRequestsTask : public EnclaveRequestTask {
    Q_OBJECT

    public:
        GetRequestsTask(SessionState *state, QObject *parent=0);
        ~GetRequestsTask() {}

    signals:
        void getRequestsComplete();
        void getRequestsFailed(const QString& error);

    public:
        const RequestList& getRequestList() const { return requestList; }
        void getRequests(const QString& requester);

    protected:
        void restComplete(const QJsonObject& resp);
        void restFailed(const QString& error);

    private:
        void setRequests(const QJsonArray& requests);

    private:
        RequestList requestList;

};

}

#endif // GETREQUESTSTASK_H
