#ifndef NICKNAMES_H
#define NICKNAMES_H

#include "Nickname.h"
#include <map>

namespace Pippip {

class Nicknames {

    public:
        Nicknames();
        ~Nicknames() {}

    public:
        Nickname& operator[] (unsigned index);
        const Nickname& operator[] (unsigned index) const;

    public:
        void append(const Nickname& nickname);
        NicknameList::iterator begin();
        NicknameList::const_iterator begin() const;
        void clear();
        NicknameList::iterator end();
        NicknameList::const_iterator end() const;
        void erase(const QString& nickname);
        bool get(const QString& name, Nickname& nickname);
        unsigned size() const;

    private:
        NicknameList nicknameList;

};

}

#endif // NICKNAMES_H
