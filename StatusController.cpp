/*
 * StatusController.cpp
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

#include "StatusController.h"
#include "mainwindow.h"
#include <QTimer>

static StatusController *theInstance = 0;

/**
 * @brief StatusController::StatusController
 * @param u
 * @param parent
 */
StatusController::StatusController(MainWindow *parent)
: QObject(parent),
  statusActive(false),
  newStatus(false),
  mainWindow(parent) {

}

/**
 * @brief StatusController::init
 * @param ui
 * @param parent
 */
void StatusController::init(MainWindow *parent) {

    if (theInstance == 0) {
        theInstance = new StatusController(parent);
    }

}

/**
 * Get the StatusController singleton.
 *
 * @brief StatusController::instance
 * @return
 */
StatusController *StatusController::instance() {

    return theInstance;

}
/**
 * @brief StatusController::timerExpired
 */
void StatusController::timerExpired() {

    if (statusActive && !newStatus) {
        mainWindow->updateStatus("", "");
        statusActive = false;
    }
    else if (newStatus) {
        newStatus = false;
        statusActive = true;
    }

}

/**
 * @brief StatusController::updateStatus
 * @param icon
 * @param status
 */
void StatusController::updateStatus(const QString &icon, const QString &status) {

    mainWindow->updateStatus(icon, status);
    if (statusActive) {
        newStatus = true;
    }
    else {
        statusActive = true;
    }
    QTimer::singleShot(5000, this, SLOT(timerExpired()));

}
