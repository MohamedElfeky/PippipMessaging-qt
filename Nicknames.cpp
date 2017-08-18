#include "Nicknames.h"

namespace Pippip {

static const Nickname EMPTY;

Nicknames::Nicknames() {
//: currentIndex(0) {
}

const Nickname& Nicknames::operator [](unsigned index) const {

    return nicknameList[index];

}

Nickname& Nicknames::operator [](unsigned index) {

    return nicknameList[index];

}

void Nicknames::append(const Nickname &nickname) {

    nicknameList.push_back(nickname);

}

NicknameList::iterator Nicknames::begin() {

    return nicknameList.begin();

}

NicknameList::const_iterator Nicknames::begin() const {

    return nicknameList.begin();

}

void Nicknames::clear() {

    nicknameList.clear();

}

NicknameList::iterator Nicknames::end() {

    return nicknameList.end();

}

NicknameList::const_iterator Nicknames::end() const {

    return nicknameList.end();

}

void Nicknames::erase(const QString &nickname) {

    NicknameList tmp;
    for (auto nick : nicknameList) {
        if (nick.entity.nickname != nickname) {
            tmp.push_back(nick);
        }
    }
    nicknameList.swap(tmp);

}

bool Nicknames::get(const QString &name, Nickname &nickname) {

    for (auto nick : nicknameList) {
        if (nick.entity.nickname == name) {
            nickname = nick;
            return true;
        }
    }

    return false;

}

unsigned Nicknames::size() const {

    return nicknameList.size();

}

}
