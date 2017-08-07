#include "Nicknames.h"

namespace Pippip {

typedef NicknameList::iterator NicknameIter;

Nicknames::Nicknames()
: currentIndex(0) {
}

void Nicknames::append(const Nickname &nickname) {

    nicknames.push_back(nickname);

}

NicknameList::iterator Nicknames::begin() {

    return nicknames.begin();

}

NicknameList::const_iterator Nicknames::begin() const {

    return nicknames.begin();

}

void Nicknames::clear() {

    nicknames.clear();

}

Nickname& Nicknames::current() {

    return nicknames[currentIndex];

}

const Nickname& Nicknames::current() const {

    return nicknames[currentIndex];

}

NicknameList::iterator Nicknames::end() {

    return nicknames.end();

}

NicknameList::const_iterator Nicknames::end() const {

    return nicknames.end();

}

unsigned Nicknames::size() const {

    return nicknames.size();

}

void Nicknames::remove(const QString &nickname) {

    NicknameIter it = nicknames.begin();
    bool removed = false;
    while (!removed && it != nicknames.end()) {
        if ((*it).entity.nickname == nickname) {
            nicknames.erase(it);
            removed = true;
        }
        ++it;
    }

}

}
