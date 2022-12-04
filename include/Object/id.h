#ifndef ID_H
#define ID_H

#include "uuid.h"
#include <ostream>

const uuids::uuid generateId();

struct Id {
    Id()
    : id(generateId())
    {}

    size_t hash(){return std::hash<uuids::uuid>{}(id);}

    const uuids::uuid id;

    bool operator==( Id const & rhs ) const {return this->id == id;}
    operator std::string(){return uuids::to_string(id);}
};

std::ostream & operator<<(std::ostream & os, Id const & value);

#endif /* ID_H */
