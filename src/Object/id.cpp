#include <Object/id.h>

#include <time.h>

namespace Hop::Object
{

    uint64_t Id::nextId = Id::NULL_ID_CODE + 1;

    std::random_device Id::rd;
    std::mt19937 Id::generator(rd());
    uuids::uuid_random_generator Id::genUUID{Id::generator};

    const uuids::uuid Id::runUUID = Id::genUUID();

    std::string to_string(const Id & i) { return std::to_string(i.id)+"-"+uuids::to_string(Id::getRunUUID()); }

    std::ostream & operator<<(std::ostream & os, Id const & value)
    {
        os << to_string(value);
        return os;
    }

}