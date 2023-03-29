#ifndef ID_H
#define ID_H

#include "uuid.h"
#include <ostream>

namespace Hop::Object
{

    class Object;

    const uuids::uuid generateId();
    const uuids::uuid NULL_UUID;

    struct Id 
    {

        friend class Hop::Object::Object;

        Id()
        : id(NULL_UUID),idStr(uuids::to_string(id))
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

    protected:

        static std::random_device rd;
        static std::mt19937 generator;
        static uuids::uuid_random_generator gen;
    };

    static const Id NULL_ID = Id(NULL_UUID);

    std::ostream & operator<<(std::ostream & os, Id const & value);

}

template<> struct std::hash<Hop::Object::Id> 
{
    std::size_t operator()(const Hop::Object::Id & i) const {return i.hash();}
};
#endif /* ID_H */
