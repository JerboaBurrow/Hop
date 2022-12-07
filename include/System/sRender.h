#ifndef SRENDER_H
#define SRENDER_H

#include <Shader/shaders.h>
/*
    System to render based on current transforms (cRenderable) by
     updating local vertex data

     struct cRenderable {
        float ox;
        float oy;
        float otheta;
        float os;
        std::string shaderHandle;
    }
*/

const size_t DEFAULT_RESERVED_SIZE = 1024;

class sRender {
public:

    sRender()
    :
    {}

    // update float verts
    void update(ObjectManager * m);

private:
    // need to also sort by object Id to use glDrawArraysInstanced??
    std::unordered_map<std::string,std::vector<float>>
}

#endif /* SRENDER_H */
