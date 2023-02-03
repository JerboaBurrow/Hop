#ifndef MAPSOURCE_H
#define MAPSOURCE_H

#include <Util/sparseData.h>
#include <utility>

using ivec2 = std::pair<int,int>;
using MapData = SparseData<ivec2,uint64_t>;

class MapSource {
public:

    MapSource()
    : data(0)
    {}

    virtual ~MapSource(){}

    virtual uint64_t getAtCoordinate(int i, int j) = 0;
    virtual void save(std::string filename) = 0;
    virtual void load(std::string filename) = 0;

private:

    MapData data;

};

#endif /* MAPSOURCE_H */
