#ifndef MAPSOURCE_H
#define MAPSOURCE_H

#include <World/mapFile.h>

namespace Hop::World 
{

    class MapSource 
    {
        
    public:

        MapSource()
        : data(MAP_DATA_NULL)
        {}

        virtual ~MapSource(){}

        virtual uint64_t getAtCoordinate(int i, int j) = 0;
        virtual void save(std::string fileNameWithoutExtension, bool compressed = true);
        virtual void load(std::string fileNameWithoutExtension, bool compressed = true);

    protected:

        MapData data;

    };

}

#endif /* MAPSOURCE_H */
