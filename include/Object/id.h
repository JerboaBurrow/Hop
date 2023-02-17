#ifndef ID_H
#define ID_H

#include "uuid.h"
#include <ostream>

const uuids::uuid generateId();
const uuids::uuid NULL_ID;

struct Id 
{
    Id()
    : id(gen()),idStr(uuids::to_string(id))
    {}

    Id(uuids::uuid i)
    : id(i),idStr(uuids::to_string(id))
    {}

    size_t hash() const {return std::hash<uuids::uuid>{}(id);}

    uuids::uuid id;
    std::string idStr;

    bool operator==( Id const & rhs ) const {return this->id == rhs.id;}
    
    bool operator<(const Id & rhs) const {return this->id < rhs.id;}

    Id & operator=(const Id & j)
    {
        id = j.id;
        idStr = j.idStr;
        return *this;
    }

private:

    static std::random_device rd;
    static std::mt19937 generator;
    static uuids::uuid_random_generator gen;
};

std::ostream & operator<<(std::ostream & os, Id const & value);

template<> struct std::hash<Id> 
{
    std::size_t operator()(const Id & i) const {return i.hash();}
};

#endif /* ID_H */
