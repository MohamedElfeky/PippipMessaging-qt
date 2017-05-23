/*
 * EnclaveRequester.h
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

#ifndef ENCLAVEREQUESTER_H
#define ENCLAVEREQUESTER_H

#include "SessionTask.h"

namespace Pippip {

struct SessionState;
class EnclaveRequest;
class EnclaveResponse;
class RESTHandler;

class EnclaveRequester : public SessionTask {
    Q_OBJECT

    public:
        EnclaveRequester(SessionState *state, QObject *parent = 0);
        ~EnclaveRequester();

    private:
        EnclaveRequester(const EnclaveRequester& other);
        EnclaveRequester& operator=(const EnclaveRequester& other);

    public slots:
        void requestComplete(RESTHandler*);
        void requestTimedOut();

    signals:
        void requestComplete(EnclaveResponse& response);
        void requestFailed(QString error);

    public:
        void request(EnclaveRequest& request);

    private:
        bool requestCompleted;

};

}

#endif // ENCLAVEREQUESTER_H
