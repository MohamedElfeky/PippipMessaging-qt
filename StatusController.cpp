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
#include "Constants.h"
#include "mainwindow.h"
#include <QTimer>

static StatusController *theInstance = 0;
static const int FADE_TIME = 10;
static const int ERROR_FADE_TIME = 20;

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
void StatusController::updateStatus(StatusSeverity sev, const QString &status) {

    QString icon = Constants::CHECK_ICON;
    fadeTime = FADE_TIME * 1000;
    switch (sev) {
        case success:
            break;
        case warn:
            icon = Constants::CAUTION_ICON;
            break;
        case info:
            icon = Constants::INFO_ICON;
            break;
        case error:
            icon = Constants::REDX_ICON;
            fadeTime = ERROR_FADE_TIME * 1000;
            break;
        case fatal:
            icon = Constants::REDBAR_ICON;
            fadeTime = ERROR_FADE_TIME * 1000;
            break;
    }

    mainWindow->updateStatus(icon, status);
    if (statusActive) {
        newStatus = true;
    }
    else {
        statusActive = true;
    }
    QTimer::singleShot(fadeTime, this, SLOT(timerExpired()));

}
