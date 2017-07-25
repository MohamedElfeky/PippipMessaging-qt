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
}

class QComboBox;
class QLineEdit;
class QRegularExpressionValidator;

class NicknameHandler : public QObject {
        Q_OBJECT

    public:
        explicit NicknameHandler(Ui::NicknamesDialog *ui, QObject *parent = 0);
        ~NicknameHandler() {}

    signals:
        void policyUpdated(const QString& policy);

    public slots:
        void nicknameAdded();
        void nicknameDeleted(const QString& name);
        void nicknamesLoaded();
        void nicknameUpdated(Pippip::Nickname nickname);
        void whitelistChanged(const Pippip::EntityList& list);

    private slots:
        void addNickname();
        void deleteNickname();
        void editPolicy(int row, int column);
        void nicknameEdited();
        void policyChanged(const QString&);
        void policySelected();

    public:
        const Pippip::Nickname& currentNickname() const;
        size_t nicknameCount() const { return nicknames.size(); }
        void setNicknames(const Pippip::NicknameList& nicknames);
        void setManager(Pippip::NicknameManager *mgr) { manager = mgr; }

    private:
        const QString& getPolicy(const QString& name) const;
        const QString& getPolicyName(const QString& policy) const;
        void loadTable();
        void policyEdited(const QString& policy);

    private:
        bool newItem;
        QString toAdd;
        Ui::NicknamesDialog *ui;
        Pippip::NicknameList nicknames;
        QComboBox *policyComboBox;
        QLineEdit *nicknameLineEdit;
        QRegularExpression nicknameRE;
        QRegularExpressionValidator *nicknameValidator;
        Pippip::NicknameManager *manager;

};

#endif // NICKNAMEHANDLER_H
