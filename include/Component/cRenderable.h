#ifndef CRENDERABLE_H
#define CRENDERABLE_H

#include <string>

namespace Hop::Object::Component
{

  struct cRenderable 
  {
      // offset in transform

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

      float uA;
      float uB;
      float uC;
      float uD;

      std::string shaderHandle;

      bool stale;

      cRenderable(std::string shader)
      : r(1.0),g(0.0),b(0.0),a(1.0),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(0.0f),uB(0.0f),uC(0.0f),uD(0.0f),
        shaderHandle(shader),stale(true)
      {}

      cRenderable(
      )
      : r(1.0),g(0.0),b(0.0),a(1.0),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(0.0f),uB(0.0f),uC(0.0f),uD(0.0f),
        shaderHandle(""),stale(true)
      {}

      cRenderable(
        std::string shader, 
        float r, float g, float b, float a,
        float ua, float ub, float uc, float ud
      )
      : r(r),g(g),b(b),a(a),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(ua),uB(ub),uC(uc),uD(ud),
        shaderHandle(shader),stale(true)
      {}

      cRenderable(
        float r, float g, float b, float a
      )
      : r(r),g(g),b(b),a(a),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(0.0f),uB(0.0f),uC(0.0f),uD(0.0f),
        shaderHandle(""),stale(true)
      {}
      
  };

}
#endif /* CRENDERABLE_H */
