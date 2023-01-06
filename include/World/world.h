#ifndef WORLD_H
#define WORLD_H

#include <gl.h>
#include <Procedural/perlin.h>
#include <texturedQuad.h>
#include <random>
#include <string>
#include <fstream>
#include <memory>
#include <Shader/shaders.h>

class CollisionDetector;

class World {
public:
    friend class CollisionDetector;

    World(uint64_t s)
    : seed(s)
    {}
    virtual void draw(Shader & s) = 0;
    virtual void save(std::string filename) = 0;
    virtual float worldUnitLength() = 0;

protected:

    uint64_t seed;
};

/*

    Notes:

    1024*1024 cells each changing every frame ~ 55fps limited by glBufferSubData
    
    Key is to avoid calculating noise samples

    1024*1024 60 fps with online sampling limit is position delta

*/

const float THRESHOLD = 0.2;

// // cells rendered with marching squares
// const uint64_t RENDER_REGION_SIZE = 128;
// // cells for use in on/offscreen dnamics (physics, rng, etc)
// // TODO mutliple levels of "dynamism"
// const uint64_t DYNAMICS_REGION_SIZE = 128*3;
// // underlying map +1 to account for marching squares
// const uint64_t RENDER_REGION_BUFFER_SIZE = RENDER_REGION_SIZE+1;
// const uint64_t DYNAMICS_REGION_BUFFER_SIZE = DYNAMICS_REGION_SIZE+1;

class PerlinWorld : public World {

public:
    PerlinWorld(uint64_t s, glm::mat4 p, uint64_t renderRegion, uint64_t dynamicsRegion);
    void draw(Shader & s);
    void save(std::string filename);
    // TexturedQuad getMap(float r = 176., float g = 176., float b = 176.);
    // TexturedQuad getLocalRegionMap();
    void updateRegion(float x, float y);
    void worldToCell(float x, float y, float & ix, float & iy);
    float worldUnitLength(){return 1.0/RENDER_REGION_SIZE;}

    ~PerlinWorld(){
        glDeleteBuffers(1,&VBOquad);
        glDeleteBuffers(1,&VBOoffset);
        glDeleteBuffers(1,&VBOid);
        glDeleteVertexArrays(1,&VAO);

        glDeleteBuffers(1,&minimapVBOoffset);
        glDeleteBuffers(1,&minimapVBOid);
        glDeleteVertexArrays(1,&minimapVAO);
    }

private:

    const uint64_t RENDER_REGION_SIZE, DYNAMICS_REGION_SIZE, RENDER_REGION_BUFFER_SIZE, DYNAMICS_REGION_BUFFER_SIZE;

    glm::mat4 projection;
    
    std::unique_ptr<bool[]> renderRegionBuffer;
    std::unique_ptr<bool[]> renderRegionBackBuffer;

    std::unique_ptr<float[]> dynamicsOffsets;
    std::unique_ptr<float[]> dynamicsIds;

    void processBufferToOffsets();

    int posX;
    int posY;

    float minimapSize = 0.25f;
    
    Perlin perlin;

    GLuint VBOquad, VBOoffset, VBOid, VAO;
    GLuint minimapVBOoffset, minimapVBOid, minimapVAO;

    std::unique_ptr<float[]> renderOffsets;
    std::unique_ptr<float[]> renderIds;

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
