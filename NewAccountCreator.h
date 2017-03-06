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
#include <string>

namespace Pippip {

class NewAccountCreator : public QObject {
    Q_OBJECT

    public:
        NewAccountCreator();
        ~NewAccountCreator();

    public:
        void createNewAccount(const std::string& accountName, const std::string& passphrase);

    private:
        NewAccountCreator(const NewAccountCreator& other);
        NewAccountCreator& operator =(const NewAccountCreator& other);

};

}

#endif // NEWACCOUNTCREATOR_H
