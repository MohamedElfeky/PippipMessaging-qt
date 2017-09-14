/*
 * StatusController.h
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

#ifndef STATUSCONTROLLER_H
#define STATUSCONTROLLER_H

#include <QObject>

class MainWindow;

class StatusController : public QObject{
        Q_OBJECT

    public:
        enum StatusSeverity { success, info, warn, error, fatal };

    private:
        StatusController(MainWindow *parent);

    public:
        ~StatusController() {}

    signals:

    public slots:
        void timerExpired();
        void updateStatus(StatusSeverity severity, const QString& status);

    public:
        static void init(MainWindow *parent);
        static StatusController *instance();    // Returns null if not initialized!

    private:
        bool statusActive;
        bool newStatus;
        int fadeTime;
        MainWindow *mainWindow;

};

#endif // STATUSCONTROLLER_H
