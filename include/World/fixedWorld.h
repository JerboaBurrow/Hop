#ifndef FIXEDWORLD_H
#define FIXEDWORLD_H

#include <World/world.h>

class FixedWorld : public World {
public:

    FixedWorld(
        uint64_t s, 
        glm::mat4 p, 
        uint64_t renderRegion, 
        uint64_t dynamicsRegion,
        uint64_t totalRegion,
        uint64_t periodicX = 0,
        uint64_t periodicY = 0
    );

    void save(std::string filename);
    void load(std::string filename);

    void updateRegion(float x, float y);

    void worldToTile(float x, float y, int & ix, int & iy);
    void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s);
    Tile tileType(int & i, int & j);
    void tileToIdCoord(int ix, int iy, int & i, int & j);

private:

    std::unique_ptr<Tile[]> worldBuffer;

    bool outOfBounds(int ix, int iy);

    uint64_t periodicX, periodicY;

    const uint64_t TOTAL_REGION_SIZE, WORLD_HALF_SIZE;

};

#endif /* FIXEDWORLD_H */
