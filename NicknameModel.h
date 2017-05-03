#ifndef NICKNAMEMODEL_H
#define NICKNAMEMODEL_H

#include <QAbstractListModel>
#include <QList>

class NicknameModel : public QAbstractListModel {
    Q_OBJECT

    public:
        explicit NicknameModel(QObject *parent = 0);

    public:
        struct Nickname {
            QString nickname;
            int policy;
        };

        static const int PUBLIC;
        static const int PRIVATE;
        static const int FRIENDSONLY;
        static const int FRIEDSOFFRIENDS;

        typedef QList<Nickname> NicknameList;

    public:
        QVariant data(const QModelIndex& index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        int rowCount(const QModelIndex&) const { return nicknames.count(); }
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        void setNicknames(const NicknameList& list) { nicknames = list; }

    private:
        QString stringifyRow(int row) const;

    private:
        NicknameList nicknames;

};

#endif // NICKNAMEMODEL_H
