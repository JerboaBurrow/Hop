#ifndef TILEWORLD_H
#define TILEWORLD_H

#include <World/world.h>

class TileWorld : public World {
public:

    TileWorld(
        uint64_t s, 
        OrthoCam & c, 
        uint64_t renderRegion, 
        uint64_t dynamicsRegion,
        uint64_t totalRegion,
        MapSource * f,
        Boundary * b
    );

    void updateRegion(float x, float y);

    void worldToTile(float x, float y, int & ix, int & iy);
    void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s);
    Tile tileType(int & i, int & j);
    void tileToIdCoord(int ix, int iy, int & i, int & j);

private:

    std::unique_ptr<Tile[]> worldBuffer;

    unsigned getWorldSizeFromFile(std::string filename);

    const uint64_t TOTAL_REGION_SIZE, WORLD_HALF_SIZE;

};

#endif /* TILEWORLD_H */
