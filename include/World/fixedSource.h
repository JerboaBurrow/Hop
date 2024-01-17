#ifndef FIXEDSOURCE_H
#define FIXEDSOURCE_H

#include <World/mapSource.h>

namespace Hop::World 
{
    class FixedSource : public MapSource 
    {

    public:

        FixedSource(){}

        uint64_t getAtCoordinate(int i, int j) { return data[ivec2(i,j)]; }
    private:

    };
}
#endif /* FIXEDSOURCE_H */
