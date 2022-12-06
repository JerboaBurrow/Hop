#ifndef OBJECTRENDERER_H
#define OBJECTRENDERER_H

#include <gl.h>
#include <memory>

class ObjectRenderer {
public:
    ObjectRenderer(){}
private:

    glm::mat4 projection;

    std::unique_ptr<float[]> offsets;
    
    float quad[6*4] = {
     // positions    texture coords
        1.0f,  1.0f, 1.0f, 1.0f,   // top right
        1.0f,  0.0f, 1.0f, 0.0f,   // bottom right
        0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
        0.0f,  1.0f, 0.0f, 1.0f,    // top left 
        0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
        1.0f,  1.0f, 1.0f, 1.0f  // top right
    };
};
#endif /* OBJECTRENDERER_H */
