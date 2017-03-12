/*
 * Fortuna.h
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

#ifndef FORTUNA_H
#define FORTUNA_H

#include <QObject>
#include <QString>

namespace Pippip {

class Fortuna : public QObject
{
        Q_OBJECT
    public:
        explicit Fortuna(QObject *parent = 0);
        ~Fortuna();

    signals:
        void error(QString errorStr);
        void finished();

    public slots:
        void run();

};

}

#endif // FORTUNA_H
