#ifndef PERLINWORLD_H
#define PERLINWORLD_H

#include <World/world.h>
#include <Procedural/perlin.h>

/*

    Notes:

    1024*1024 cells each changing every frame ~ 55fps limited by glBufferSubData
    
    Key is to avoid calculating noise samples

    1024*1024 60 fps with online sampling limit is position delta

*/

const float THRESHOLD = 0.2;

class PerlinWorld : public World {

public:

    PerlinWorld(uint64_t s, glm::mat4 p, uint64_t renderRegion, uint64_t dynamicsRegion);
    
    void load(std::string filename){};
    void save(std::string filename){};
    void updateRegion(float x, float y);

    void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s);
    Tile tileType(int & i, int & j);
    void tileToIdCoord(int ix, int iy, int & i, int & j);

    std::pair<float,float> getPos(){float u = worldUnitLength(); return std::pair<float,float>(u*tilePosX,u*tilePosY);}

private:

    const uint64_t RENDER_REGION_BUFFER_SIZE, DYNAMICS_REGION_BUFFER_SIZE;

    std::unique_ptr<bool[]> renderRegionBuffer;
    std::unique_ptr<bool[]> renderRegionBackBuffer;

    void processBufferToOffsets();
    
    Perlin perlin;

};

#endif /* PERLINWORLD_H */
