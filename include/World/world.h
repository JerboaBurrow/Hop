#ifndef WORLD_H
#define WORLD_H

#include <gl.h>
#include <random>
#include <string>
#include <fstream>
#include <memory>
#include <Shader/shaders.h>
#include <World/tile.h>
#include <exception>
#include <orthoCam.h>
#include <World/boundary.h>
#include <World/mapSource.h>
#include <World/fixedSource.h>

class CollisionDetector;

class MapReadException: public std::exception {
public:
    MapReadException(std::string msg)
    : msg(msg)
    {}
private:
    virtual const char * what() const throw(){
        return msg.c_str();
    }
    std::string msg;
};

class MapWriteException: public std::exception {
public:
    MapWriteException(std::string msg)
    : msg(msg)
    {}
private:
    virtual const char * what() const throw(){
        return msg.c_str();
    }
    std::string msg;
};

class World {
public:

    World(
        uint64_t s, 
        OrthoCam & c, 
        uint64_t renderRegion, 
        uint64_t dynamicsRegion,
        MapSource * f,
        Boundary * b
    );

    virtual void draw(Shader & s);

    virtual void save(std::string fileNameWithoutExtension, bool compressed = true){map->save(fileNameWithoutExtension, compressed); forceUpdate = true;}
    virtual void load(std::string fileNameWithoutExtension, bool compressed = true){map->load(fileNameWithoutExtension, compressed); forceUpdate = true;}

    float worldUnitLength(){return 1.0/RENDER_REGION_SIZE;}

    void worldToTile(float x, float y, int & ix, int & iy);

    virtual void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s) = 0;
    virtual Tile tileType(int & i, int & j) = 0;
    virtual void tileToIdCoord(int ix, int iy, int & i, int & j) = 0;

    virtual bool pointOutOfBounds(float x, float y);
    virtual bool cameraOutOfBounds(float x, float y);

    virtual void updateRegion(float x, float y) = 0;

    std::pair<float,float> getPos(){float u = worldUnitLength(); return std::pair<float,float>(u*tilePosX,u*tilePosY);}
    std::pair<float,float> getMapCenter(){
        float u = worldUnitLength(); 
        return std::pair<float,float>(u*(tilePosX+RENDER_REGION_SIZE/2.0),u*(tilePosY+RENDER_REGION_SIZE/2.0));
    }

    ~World(){
        glDeleteBuffers(1,&VBOquad);
        glDeleteBuffers(1,&VBOoffset);
        glDeleteBuffers(1,&VBOid);
        glDeleteVertexArrays(1,&VAO);
    }

protected:

    uint64_t seed;
    
    const uint64_t RENDER_REGION_SIZE, DYNAMICS_REGION_SIZE;

    OrthoCam & camera;

    glm::mat4 projection;

    std::unique_ptr<float[]> dynamicsOffsets;
    std::unique_ptr<float[]> dynamicsIds;

    std::unique_ptr<float[]> renderOffsets;
    std::unique_ptr<float[]> renderIds;

    int posX;
    int posY;

    int tilePosX, tilePosY;

    bool forceUpdate;

    GLuint VBOquad, VBOoffset, VBOid, VAO;

    Boundary * boundary;

    MapSource * map;

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
