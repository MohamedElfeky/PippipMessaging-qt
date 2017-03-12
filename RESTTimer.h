/*
 * RESTTimer.h
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

#ifndef RESTTIMER_H
#define RESTTIMER_H

#include <QObject>

class QEventLoop;
class QTimer;

namespace Pippip {

class RESTTimer : public QObject {
        Q_OBJECT

    public:
        explicit RESTTimer(QObject *parent=0);
        ~RESTTimer();

    public:
        void addSource(QObject *source, const char *signal);
        void stop();
        bool wait(int milliseconds);

    public slots:
        void timeout();

    private:
        bool timedout;
        QEventLoop *eventLoop;
        QTimer *timer;

};

}

#endif // RESTTIMER_H
