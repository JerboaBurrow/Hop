#ifndef WORLD_H
#define WORLD_H

#include <gl.h>
#include <Procedural/perlin.h>
#include <texturedQuad.h>
#include <random>
#include <string>
#include <fstream>
#include <memory>

/*

    Notes:

    1024*1024 cells each changing every frame ~ 55fps limited by glBufferSubData
    
    Key is to avoid calculating noise samples

    1024*1024 60 fps with online sampling limit is position delta

*/

// cells rendered with marching squares
const uint64_t REGION_SIZE = 128;
// underlying map +1 to account for marching squares
const uint64_t REGION_BUFFER_SIZE = REGION_SIZE+1;
const uint64_t MAX_DELTA = 8;
const uint64_t SHELL_SIZE = REGION_SIZE*4;
const float THRESHOLD = 0.2;

class World {

public:
    World(uint64_t s, glm::mat4 p);
    void draw();
    TexturedQuad getMap(float r = 176., float g = 176., float b = 176.);
    TexturedQuad getLocalRegionMap();
    void save(std::string filename);
    void updateRegion(float x, float y);
    void worldToCell(float x, float y, float & ix, float & iy);

    ~World(){
        glDeleteProgram(shader);
        glDeleteBuffers(1,&VBOquad);
        glDeleteBuffers(1,&VBOoffset);
        glDeleteBuffers(1,&VBOid);
        glDeleteVertexArrays(1,&VAO);
    }

private:

    uint64_t seed;
    std::unique_ptr<bool[]> regionBuffer;
    std::unique_ptr<bool[]> regionBackBuffer;
    void processBufferToOffsets();

    int posX;
    int posY;

    Perlin perlin;

    GLuint shader, VBOquad, VBOoffset, VBOid, VAO;

    glm::mat4 projection;

    std::unique_ptr<float[]> offsets;
    std::unique_ptr<float[]> ids;

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

    void getWorldShell(int ox, int oy);

};

#endif /* WORLD_H */
