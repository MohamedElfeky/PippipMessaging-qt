#ifndef NICKNAMES_H
#define NICKNAMES_H

#include "Nickname.h"

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
        Nickname& current();
        const Nickname& current() const;
        NicknameList::iterator end();
        NicknameList::const_iterator end() const;
        unsigned getCurrentIndex() const { return currentIndex; }
        void setCurrentIndex(unsigned index) { currentIndex = index; }
        void remove(const QString& nickname);
        unsigned size() const;

    private:
        unsigned currentIndex;
        NicknameList nicknames;

};

}

#endif // NICKNAMES_H
