#ifndef WORLD_H
#define WORLD_H

#include <gl.h>
#include <random>
#include <string>
#include <fstream>
#include <memory>
#include <Shader/shaders.h>
#include <World/tile.h>

class CollisionDetector;

class World {
public:

    World(uint64_t s, glm::mat4 p, uint64_t renderRegion, uint64_t dynamicsRegion);

    virtual void draw(Shader & s);

    virtual void save(std::string filename) = 0;
    virtual void load(std::string filename) = 0;

    float worldUnitLength(){return 1.0/RENDER_REGION_SIZE;}

    void worldToTile(float x, float y, int & ix, int & iy);

    virtual void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s) = 0;
    virtual Tile tileType(int & i, int & j) = 0;
    virtual void tileToIdCoord(int ix, int iy, int & i, int & j) = 0;

    virtual void updateRegion(float x, float y) = 0;

    ~World(){
        glDeleteBuffers(1,&VBOquad);
        glDeleteBuffers(1,&VBOoffset);
        glDeleteBuffers(1,&VBOid);
        glDeleteVertexArrays(1,&VAO);
    }

protected:


    uint64_t seed;
    
    const uint64_t RENDER_REGION_SIZE, DYNAMICS_REGION_SIZE;

    glm::mat4 projection;

    std::unique_ptr<float[]> dynamicsOffsets;
    std::unique_ptr<float[]> dynamicsIds;

    std::unique_ptr<float[]> renderOffsets;
    std::unique_ptr<float[]> renderIds;

    int posX;
    int posY;

    int tilePosX, tilePosY;

    GLuint VBOquad, VBOoffset, VBOid, VAO;

    float quad[6*4] = {
    // positions  / texture coords
     1.0f,  1.0f, 1.0f, 1.0f,   // top right
     1.0f,  0.0f, 1.0f, 0.0f,   // bottom right
     0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
     0.0f,  1.0f, 0.0f, 1.0f,    // top left 
     0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
     1.0f,  1.0f, 1.0f, 1.0f  // top right
    };
    std::default_random_engine generator;
};

#endif /* WORLD_H */
