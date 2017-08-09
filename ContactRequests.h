#ifndef CONTACTREQUESTS_H
#define CONTACTREQUESTS_H

#include "ContactRequest.h"

namespace Pippip {

class ContactRequests {

    public:
        ContactRequests();
        ~ContactRequests() {}

    public:
        const ContactRequestIn& operator[] (int index) const;

    public:
        void add(const ContactRequestIn& request);
        const RequestList::const_iterator begin() const;
        void clear();
        const RequestList::const_iterator end() const;
        unsigned size() const;

    private:
        RequestList requests;

};

}

#endif // CONTACTREQUESTS_H
