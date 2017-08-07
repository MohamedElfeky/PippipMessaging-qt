/*
 * NicknameHandler.h
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

#ifndef NICKNAMEHANDLER_H
#define NICKNAMEHANDLER_H

#include "Nickname.h"
#include <QObject>
#include <QRegularExpression>

namespace Ui {
    class NicknamesDialog;
}

namespace Pippip {
    class NicknameManager;
    class Nicknames;
}

class QComboBox;
class QLineEdit;
class QRegularExpressionValidator;

class NicknameHandler : public QObject {
        Q_OBJECT

    public:
        explicit NicknameHandler(Ui::NicknamesDialog *ui, QObject *parent = 0);
        ~NicknameHandler() {}

    public slots:
        void nicknameAdded(const QString& status);
        void nicknameDeleted(const QString& status);
        void nicknamesLoaded();
        //void nicknameNotFound();
        void policyUpdated(const QString& response);
        void taskFailed(const QString& taskName);

    private slots:
        void addNickname();
        void deleteNickname();
        void editPolicy(int row, int column);
        void nicknameEdited();
        void policyEdited(Qt::Key);

    public:
        void setManager(Pippip::NicknameManager *manager);

    private:
        const QString& getPolicy(const QString& name) const;
        const QString& getPolicyName(const QString& policy) const;
        void loadTable();

    private:
        bool firstLoad;
        bool newItem;
        Ui::NicknamesDialog *ui;
        QComboBox *policyComboBox;
        QLineEdit *nicknameLineEdit;
        QRegularExpression nicknameRE;
        QRegularExpressionValidator *nicknameValidator;
        Pippip::NicknameManager *nicknameManager;
        Pippip::Nicknames *nicknames;
        QString working;
        Pippip::Nickname undo;

};

#endif // NICKNAMEHANDLER_H
