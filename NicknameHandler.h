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
    class EnclaveRequestTask;
    class SessionState;
}

class QComboBox;
class QLineEdit;
class QRegularExpressionValidator;

class NicknameHandler : public QObject {
        Q_OBJECT

    public:
        explicit NicknameHandler(Ui::NicknamesDialog *ui, QObject *parent = 0);
        ~NicknameHandler() {}

    private slots:
        void addNickname();
        void deleteNickname();
        void editPolicy(int row, int column);
        void nicknameEdited();
        void policyEdited(Qt::Key);
        void requestComplete(Pippip::EnclaveRequestTask *task);
        void requestFailed(const QString& error);

    public:
        void loadNicknames(Pippip::SessionState *state);
        void loadTable(int start = 0);
        int getSelectedRow() const { return selectedRow; }
        void setSelectedRow(int row) { selectedRow = row; }

    private:
        const QString& getPolicy(const QString& name) const;
        const QString& getPolicyName(const QString& policy) const;

    private:
        bool nicknamesValid;
        bool newItem;
        int selectedRow;
        QString taskName;
        Ui::NicknamesDialog *ui;
        QComboBox *policyComboBox;
        QLineEdit *nicknameLineEdit;
        QRegularExpression nicknameRE;
        QRegularExpressionValidator *nicknameValidator;
        Pippip::SessionState *state;
        Pippip::Nickname working;

};

#endif // NICKNAMEHANDLER_H
