#include <World/fixedSource.h>

namespace Hop::World 
{

    uint64_t FixedSource::getAtCoordinate(int i, int j)
    {
        return data[ivec2(i,j)];
    }

}