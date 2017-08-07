/*
 * ContactAddKeyFilter.cpp
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

#include "KeyFilter.h"
#include <QEvent>
#include <QKeyEvent>

KeyFilter::KeyFilter(QObject *parent)
: QObject(parent) {
}

void KeyFilter::addKey(Qt::Key key) {

    keyList.push_back(key);

}

bool KeyFilter::eventFilter(QObject* obj, QEvent* event) {

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        int eventKey = keyEvent->key();
        for (auto key : keyList) {
            if (eventKey == key) {
                emit keyPressed(key);
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);

}
