#include "ContactRequests.h"

namespace Pippip {

ContactRequests::ContactRequests() {

}

const ContactRequestIn& ContactRequests::operator [] (int index) const {

    return requests[index];

}

void ContactRequests::add(const ContactRequestIn &request) {

    requests.push_back(request);

}

const RequestList::const_iterator ContactRequests::begin() const {

    return requests.begin();

}

void ContactRequests::clear() {

    requests.clear();

}

const RequestList::const_iterator ContactRequests::end() const {

    return requests.end();

}

unsigned ContactRequests::size() const {

    return requests.size();

}

}

