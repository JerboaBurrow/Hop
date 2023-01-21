#ifndef FIXEDWORLD_H
#define FIXEDWORLD_H

#include <World/world.h>

class FixedWorld : public World {
public:

    FixedWorld(uint64_t n, glm::mat4 p);

    void draw(Shader & s);

    void save(std::string filename);
    void load(std::string filename);

    float worldUnitLength(){1.0/WORLD_SIZE;}
    // world coord to tile index (may of may not be buffered)
    virtual void worldToTile(float x, float y, int & ix, int & iy) = 0;
    // data for given tile
    virtual void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s) = 0;
    virtual Tile tileType(int & i, int & j) = 0;

private:

    uint64_t WORLD_SIZE;
}

#endif /* FIXEDWORLD_H */
