#ifndef ID_H
#define ID_H

#include "uuid.h"
#include <ostream>

const uuids::uuid generateId();

struct Id {
    Id()
    : id(generateId())
    {}

    size_t hash() const {return std::hash<uuids::uuid>{}(id);}

    const uuids::uuid id;

    bool operator==( Id const & rhs ) const {return this->id == rhs.id;}
    operator std::string(){return uuids::to_string(id);}

    bool operator<(const Id & rhs) const {
        return this->id < rhs.id;
    }
};

std::ostream & operator<<(std::ostream & os, Id const & value);

template<> struct std::hash<Id> {
    std::size_t operator()(const Id & i) const {return i.hash();}
};

#endif /* ID_H */
