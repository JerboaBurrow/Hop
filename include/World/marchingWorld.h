#ifndef MARCHINGWORLD_H
#define MARCHINGWORLD_H

#include <World/world.h>
#include <World/mapSource.h>
#include <World/perlinSource.h>
#include <World/fixedSource.h>

class MarchingWorld : public World {
    public:

    MarchingWorld(
        uint64_t s, 
        OrthoCam & c, 
        uint64_t renderRegion, 
        uint64_t dynamicsRegion,
        MapSource * f
    );

    void save(std::string filename){}
    void load(std::string filename){}

    void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s);
    Tile tileType(int & i, int & j);
    void tileToIdCoord(int ix, int iy, int & i, int & j);
    void updateRegion(float x, float y);

private:

    const uint64_t RENDER_REGION_BUFFER_SIZE, DYNAMICS_REGION_BUFFER_SIZE;

    std::unique_ptr<bool[]> renderRegionBuffer;
    std::unique_ptr<bool[]> renderRegionBackBuffer;

    void processBufferToOffsets();

    MapSource * map;

};

#endif /* MARCHINGWORLD_H */
