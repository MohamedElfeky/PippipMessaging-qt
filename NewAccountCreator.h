/*
 * NewAccountCreator.h
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

#ifndef NEWACCOUNTCREATOR_H
#define NEWACCOUNTCREATOR_H

#include "SessionTask.h"
#include <QString>
#include <string>

class NewAccountDialog;

namespace Pippip {

class ParameterGenerator;
class RESTHandler;
class PostPacket;

class NewAccountCreator : public SessionTask {
    Q_OBJECT

    public:
        explicit NewAccountCreator(NewAccountDialog *parent, ParameterGenerator *gen);
        ~NewAccountCreator();

    private:
        NewAccountCreator(const NewAccountCreator& other);
        NewAccountCreator& operator =(const NewAccountCreator& other);

    signals:
        void accountComplete(int response);
        void incrementProgress(int incr);
        void resetProgress();
        void updateInfo(QString info);

    public slots:
        void finishComplete(RESTHandler*);
        void finishTimedOut();
        void requestComplete(RESTHandler*);
        void requestTimedOut();

    public:
        void createNewAccount(const QString& accountName, const QString& passphrase);

    protected:
        void sessionComplete(const QString& result);

    private:
        void doRequest();
        void doFinish();
        void requestFailed(const QString& error);

    private:
        //enum RequestState { initial, get_session, request, finish, complete, failed, timed_out };
        //RequestState requestState;
        bool timedOut;
        bool responseComplete;
        bool finalComplete;
        std::string accountName;
        std::string passphrase;
        NewAccountDialog *dialog;
        ParameterGenerator *generator;

};

}

#endif // NEWACCOUNTCREATOR_H
