#ifndef ID_H
#define ID_H

#include "uuid.h"
#include <ostream>

const uuids::uuid generateId();

struct Id {
    Id()
    : id(generateId()),idStr(uuids::to_string(id))
    {}

    Id(uuids::uuid i)
    : id(i),idStr(uuids::to_string(id))
    {}

    size_t hash() const {return std::hash<uuids::uuid>{}(id);}

    const uuids::uuid id;
    const std::string idStr;

    bool operator==( Id const & rhs ) const {return this->id == rhs.id;}
    
    bool operator<(const Id & rhs) const {
        return this->id < rhs.id;
    }

    Id operator=(const Id & j){
        return Id(j.id);
    }
};

std::ostream & operator<<(std::ostream & os, Id const & value);

template<> struct std::hash<Id> {
    std::size_t operator()(const Id & i) const {return i.hash();}
};

#endif /* ID_H */
