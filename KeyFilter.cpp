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

#include "ContactAddKeyFilter.h"
#include <QEvent>
#include <QKeyEvent>

ContactAddKeyFilter::ContactAddKeyFilter(QObject *parent)
: QObject(parent) {
}

bool ContactAddKeyFilter::eventFilter(QObject* obj, QEvent* event) {

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
            case Qt::Key_Tab:
                emit tabPressed();
                return true;
            case Qt::Key_Return:
            case Qt::Key_Enter:
                emit enterPressed();
                return true;
        }
    }

    return QObject::eventFilter(obj, event);

}
