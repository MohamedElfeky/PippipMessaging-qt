/*
 * UDPListener.h
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

#ifndef UDPLISTENER_H
#define UDPLISTENER_H

#include <QObject>
#include <QString>

namespace Pippip {

class EntropyStream;

class UDPListener : public QObject {
    Q_OBJECT

    public:
        explicit UDPListener(EntropyStream *stream, QObject *parent=0);
        ~UDPListener();

    private:
        UDPListener(const UDPListener& other);
        UDPListener& operator= (const UDPListener& other);

    signals:
        void error(QString errstr, bool fatal);
        void finished();

    public slots:
        void runListener();

    private:
        void quit();
        bool createSocket();

    private:
        bool run;
        EntropyStream *stream;
        int socket;

};

}

#endif // UDPLISTENER_H
