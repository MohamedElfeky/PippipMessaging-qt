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

#include <QObject>
#include <QString>
#include <string>

class NewAccountDialog;

namespace Pippip {

class SessionState;
class ParameterGenerator;
class RESTHandler;

class NewAccountCreator : QObject {
    Q_OBJECT

    public:
        explicit NewAccountCreator(NewAccountDialog *parent, SessionState *session);
        ~NewAccountCreator();

    private:
        NewAccountCreator(const NewAccountCreator& other);
        NewAccountCreator& operator =(const NewAccountCreator& other);

    signals:
        void accountComplete();
        void incrementProgress(int incr);
        void updateInfo(QString info);

    public slots:
        void finalComplete(RESTHandler*);
        void requestComplete(RESTHandler*);
        void requestFailed(RESTHandler*);
        void sessionStarted();
        void sessionFailed(QString error);

    public:
        void createNewAccount(const QString& accountName, const QString& passphrase);
        void progress();

    private:
        void doAccountRequest();
        void doFinish();
        bool startSession();

    private:
        SessionState *session;
        NewAccountDialog *dialog;
        std::string accountName;
        std::string passphrase;
        ParameterGenerator *generator;

};

}

#endif // NEWACCOUNTCREATOR_H
