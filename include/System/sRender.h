#ifndef SRENDER_H
#define SRENDER_H

#include <Shader/shaders.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <Component/cRenderable.h>
#include <System/system.h>
#include <Object/objectManager.h>
#include <gl.h>

const int MAX_OBJECTS_PER_SHADER = 100000; // 0.4 megabytes per shader

class ObjectManager;
class sRender;

class sRender : public System {
public:

    sRender()
    {
        initialise();
    }

    ~sRender();

    // update float verts, loop over all object
    //  with matching signature
    void update(ObjectManager * m, Shaders * s);
    void draw(Shaders * s, bool debug = false);
    void initialise();

private:

    void addNewShader(std::string handle);
    void addNewObject(Id i, std::string handle);
    void moveOffsets(Id i, std::string oldShader, std::string newShader);
    void updateOffsets(std::string handle);
    void updateColours(std::string handle);
    void updateTexOffsets(std::string handle);
    
    /*
        Keep offsets in contiguous memory per shader
         record also a mapping from an object to a its
         offsets by marking shader name and index
    */
    std::unordered_map<
        std::string,
        std::pair<std::size_t,std::vector<float>>
    > offsets;

    std::unordered_map<
        Id,
        std::pair<std::string,std::size_t>
    > idToIndex;

    // an unordered map will require a 
    //  custom hash function for a pair key
    std::map<
        std::pair<std::string,std::size_t>,
        Id
    > indexToId;

    std::unordered_map<
        std::string,
        std::pair<GLuint,std::vector<GLuint>>
    > shaderBufferObjects;

    GLuint quadVBO;

    float quad[6*4] = {
        // positions  / texture coords
        1.0f,  1.0f, 1.0f, 1.0f,   // top right
        1.0f,  0.0f, 1.0f, 0.0f,   // bottom right
        0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
        0.0f,  1.0f, 0.0f, 1.0f,    // top left 
        0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
        1.0f,  1.0f, 1.0f, 1.0f  // top right
    };

};

#endif /* SRENDER_H */
