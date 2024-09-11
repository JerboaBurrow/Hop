#ifndef CRENDERABLE_H
#define CRENDERABLE_H

#include <string>

namespace Hop::Object::Component
{

  struct cRenderable
  {
      glm::vec4 colour;

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

      uint64_t priority;

      std::string shaderHandle;

      bool stale;

      cRenderable(std::string shader)
      : colour(0,0,0,1),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(0.0f),uB(0.0f),uC(0.0f),uD(0.0f),
        priority(0),
        shaderHandle(shader),stale(true)
      {}

      cRenderable(
      )
      : colour(0,0,0,1),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(0.0f),uB(0.0f),uC(0.0f),uD(0.0f),
        priority(0),
        shaderHandle(""),stale(true)
      {}

      cRenderable(
        std::string shader,
        float r, float g, float b, float a,
        float ua, float ub, float uc, float ud,
        uint64_t p = 0
      )
      : colour(r, g, b, a),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(ua),uB(ub),uC(uc),uD(ud),
        priority(p),
        shaderHandle(shader),stale(true)
      {}

      cRenderable(
        float r, float g, float b, float a,
        uint64_t p = 0
      )
      : colour(r, g, b, a),
        ux(0.0),uy(0.0),vx(0.0),vy(0.0),
        uA(0.0f),uB(0.0f),uC(0.0f),uD(0.0f),
        priority(p),
        shaderHandle(""),stale(true)
      {}
  };

}
#endif /* CRENDERABLE_H */
