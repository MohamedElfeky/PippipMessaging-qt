#ifndef NICKNAMEMODEL_H
#define NICKNAMEMODEL_H

#include <QAbstractTableModel>
#include <vector>
#include <map>

class NicknameModel : public QAbstractTableModel {
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

        typedef std::vector<Nickname> NicknameList;

    public:
        void addNickname(const Nickname& nickname);
        int columnCount(const QModelIndex&) const { return 2; }
        QVariant data(const QModelIndex& index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        //bool insertRows(int row, int count, const QModelIndex &parent);
        int rowCount(const QModelIndex&) const { return nicknames.size(); }
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        void setNicknames(const NicknameList& list);

    private:
        QString nicknameValue(int row, int column)const;

    private:
        typedef std::map<QString, int> NicknameMap;
        typedef NicknameMap::iterator NickIter;
        typedef NicknameMap::const_iterator NickConstIter;

        NicknameList nicknameList;
        NicknameMap nicknames;

};

#endif // NICKNAMEMODEL_H
