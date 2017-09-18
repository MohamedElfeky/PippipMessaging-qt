/*
 * RequestContactTask.h
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

#ifndef REQUESTCONTACTTASK_H
#define REQUESTCONTACTTASK_H

#include "EnclaveRequestTask.h"

namespace Pippip {

struct ContactRequestOut;

class RequestContactTask : public EnclaveRequestTask {
        Q_OBJECT

    public:
        RequestContactTask(SessionState *state, QObject *parent = 0);
        ~RequestContactTask() {}

    signals:
        void requestContactComplete();
        void requestContactFailed(const QString& error);

    public:
        long getRequestId() const { return requestId; }
        const QString& getStatus() const { return status; }
        void requestContact(const ContactRequestOut& out);

    protected:
        void restComplete(const QJsonObject& resp);
        void restFailed(const QString& error);
        void restTimedOut();

    private:
        long requestId;
        QString status;

};

}

#endif // REQUESTCONTACTTASK_H
