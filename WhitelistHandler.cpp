/*
 * WhitelistHandler.cpp
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

#include "WhitelistHandler.h"
#include "ui_NicknamesDialog.h"

WhitelistHandler::WhitelistHandler(Ui::NicknamesDialog *u, QObject *parent)
: QObject(parent),
  ui(u) {

}

/*
 * This synchronizes the table widget to the nicknames list.
 */
void WhitelistHandler::loadTable() {

    ui->whitelistTableWidget->clearContents();
    ui->whitelistTableWidget->setRowCount(whitelist.size());
    int row = 0;
    for (auto entity : whitelist) {
        QLabel *nameLabel = new QLabel(entity.nickname);
        nameLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->whitelistTableWidget->setCellWidget(row, 0, nameLabel);
        QLabel *puidLabel = new QLabel(entity.publicId);
        puidLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->nicknameTableWidget->setCellWidget(row++, 1, puidLabel);
    }
    ui->addFriendButton->setEnabled(true);
}

void WhitelistHandler::setWhitelist(const Pippip::EntityList &list) {

    whitelist = list;
    loadTable();

}
