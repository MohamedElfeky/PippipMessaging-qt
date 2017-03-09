/*
 * RESTTimer.cpp
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

#include "RESTTimer.h"
#include <QEventLoop>
#include <QTimer>

namespace Pippip {

RESTTimer::RESTTimer(QObject *parent, const char *signal)
: QObject(parent),
  timedout(false),
  eventLoop(new QEventLoop(this)),
  timer(new QTimer(this)) {

    connect(parent, signal, eventLoop, SLOT(quit()));

}

RESTTimer::~RESTTimer() {

}

void RESTTimer::timeout() {

    timedout = true;
    eventLoop->quit();

}

bool RESTTimer::wait(int milliseconds) {

    timedout = false;
    if (milliseconds != 0) {
        timer->setInterval(milliseconds);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
        timer->start();
    }
    // Else wait forever.

    eventLoop->exec();

    if (!timedout) {
        timer->stop();
    }

    return !timedout;

}

}
