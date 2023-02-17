#include <Object/id.h>

#include <time.h>

std::random_device Id::rd;
std::mt19937 Id::generator(rd());
uuids::uuid_random_generator Id::gen{Id::generator};

std::ostream & operator<<(std::ostream & os, Id const & value)
{
    os << uuids::to_string(value.id);
    return os;
}