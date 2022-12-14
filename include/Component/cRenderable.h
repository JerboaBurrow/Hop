#ifndef CRENDERABLE_H
#define CRENDERABLE_H

#include <string>

struct cRenderable {
    // offset
    float ox;
    float oy;
    float otheta;
    float os;

    // colour
    float r;
    float g;
    float b;
    float a;

    // atlas coord
    float ux;
    float uy;
    
    // extra?
    float vx;
    float vy;
    std::string shaderHandle;

    bool stale;

    cRenderable(std::string shader)
    : ox(0.0),oy(0.0),otheta(0.0),os(1.0),
      r(1.0),g(0.0),b(0.0),a(1.0),
      ux(0.0),uy(0.0),vx(0.0),vy(0.0),
      shaderHandle(shader),stale(true)
    {}

    cRenderable(
        float x,
        float y,
        std::string shader
    )
    : ox(x),oy(y),otheta(0.0),os(1.0),
      r(1.0),g(0.0),b(0.0),a(1.0),
      ux(0.0),uy(0.0),vx(0.0),vy(0.0),
      shaderHandle(shader),stale(true)
    {}

    cRenderable(
        float x,
        float y,
        float s,
        std::string shader
    )
    : ox(x),oy(y),otheta(0.0),os(s),
      r(1.0),g(0.0),b(0.0),a(1.0),
      ux(0.0),uy(0.0),vx(0.0),vy(0.0),
      shaderHandle(shader),stale(true)
    {}
    
    cRenderable(
    )
    : ox(0.0),oy(0.0),otheta(0.0),os(1.0),
      r(1.0),g(0.0),b(0.0),a(1.0),
      ux(0.0),uy(0.0),vx(0.0),vy(0.0),
      shaderHandle(""),stale(true)
    {}
     
};

#endif /* CRENDERABLE_H */
