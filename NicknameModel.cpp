#include "NicknameModel.h"
#include "EditNicknameDialog.h"

// Keep the policy indexes in sync
const int NicknameModel::PRIVATE = EditNicknameDialog::PRIVATE;
const int NicknameModel::PUBLIC = EditNicknameDialog::PUBLIC;
const int NicknameModel::FRIENDSONLY = EditNicknameDialog::FRIENDSONLY;
const int NicknameModel::FRIEDSOFFRIENDS = EditNicknameDialog::FRIEDSOFFRIENDS;

static QString policyNames[] = { "Private", "Public", "Friends Only", "Friends of Friends" };

NicknameModel::NicknameModel(QObject *parent)
: QAbstractListModel(parent) {
}

QVariant NicknameModel::data(const QModelIndex &index, int role) const {

    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            return stringifyRow(index.row());
            break;
        default:
            return QVariant();
    }

}

Qt::ItemFlags NicknameModel::flags(const QModelIndex &index) const {
    return QAbstractItemModel::flags(index);
}

bool NicknameModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    return false;
}

QString NicknameModel::stringifyRow(int row) const {

    QString line;
    line.append(nicknames[row].nickname);
    line.append(" - ");
    line.append(policyNames[nicknames[row].policy]);
    return line;

}
