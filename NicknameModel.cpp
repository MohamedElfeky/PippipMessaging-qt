#include "NicknameModel.h"
#include "EditNicknameDialog.h"
#include <iostream>

// Keep the policy indexes in sync
const int NicknameModel::PRIVATE = EditNicknameDialog::PRIVATE;
const int NicknameModel::PUBLIC = EditNicknameDialog::PUBLIC;
const int NicknameModel::FRIENDSONLY = EditNicknameDialog::FRIENDSONLY;
const int NicknameModel::FRIEDSOFFRIENDS = EditNicknameDialog::FRIEDSOFFRIENDS;

static QString policyNames[] = { "Private", "Public", "Friends Only", "Friends of Friends" };
static QString headers[] = { "Nickname", "Policy" };

NicknameModel::NicknameModel(QObject *parent)
: QAbstractTableModel(parent) {
}

void NicknameModel::addNickname(const Nickname &nickname) {

    NickIter it = nicknames.find(nickname.nickname);
    if (it == nicknames.end()) {
        QModelIndex after = createIndex(nicknames.size() - 1, 0);
        nicknameList.push_back(nickname);
        nicknames[nickname.nickname] = nickname.policy;
        QModelIndex topLeft = createIndex(nicknames.size() - 1, 0);
        QModelIndex bottomRight = createIndex(nicknames.size() - 1, 1);
        emit dataChanged(topLeft, bottomRight);
    }
    // TODO Throw an exception?

}

QVariant NicknameModel::data(const QModelIndex &index, int role) const {

    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::TextAlignmentRole:
            return int(Qt::AlignCenter | Qt::AlignVCenter);
        case Qt::DisplayRole:
            return nicknameValue(index.row(), index.column());
            break;
        default:
            return QVariant();
    }

}

Qt::ItemFlags NicknameModel::flags(const QModelIndex &index) const {

    Qt::ItemFlags itemFlags = QAbstractItemModel::flags(index);
    if (index.row() == 1) {
        itemFlags |= Qt::ItemIsEditable;
    }
    return itemFlags;

}

QVariant NicknameModel::headerData(int section, Qt::Orientation, int role) const {

    if (role == Qt::DisplayRole) {
        return headers[section];
    }
    else  {
        return QVariant();
    }

}

bool NicknameModel::insertRows(int row, int count, const QModelIndex &parent) {
    std::cout << "Insert rows called";
}

QString NicknameModel::nicknameValue(int row, int column) const {

    QString nickname = nicknameList[row].nickname;
    if (column == 0) {
        return nickname;
    }
    else if (column == 1) {
       NickConstIter it = nicknames.find(nickname);
       return policyNames[it->second];
    }
    else {
        return "";
    }

}

bool NicknameModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    if (!index.isValid() || index.column() == 0) {
        return false;
    }

    if (role == Qt::EditRole) {
        QString nickname = nicknameList[index.row()].nickname;
        QString policy = value.toString();
        NickIter it = nicknames.find(nickname);
        for (int n = 0; n < 4; ++n) {
            if (policy == policyNames[n]) {
                it->second = n;
                emit dataChanged(index, index);
            }
        }
        return true;
    }
    else {
        return false;
    }

}

void NicknameModel::setNicknames(const NicknameList &list) {

    nicknameList = list;
    nicknames.clear();
    for (Nickname nickname : nicknameList) {
        nicknames[nickname.nickname] = nickname.policy;
    }

}
