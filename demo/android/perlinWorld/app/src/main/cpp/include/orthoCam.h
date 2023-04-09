#ifndef ORTHOCAM_H
#define ORTHOCAM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Hop::System::Rendering
{
  /*
    An orthographic camera.

    Assumes world coordinates: [0,1]x[0,1]
  */
  class OrthoCam 
  {

  public:

    OrthoCam(int resx, int resy)
    : resolution(resx,resy), zoomLevel(1.0f), position(glm::vec2(0.0,0.0)) 
    {
      viewPort = glm::vec4(0,resx,0,resy);
      update();
      glViewport(0,0,resx,resy);
    }

    OrthoCam(int resx, int resy, glm::vec2 pos)
    : resolution(resx,resy), zoomLevel(1.0f), position(pos) 
    {
      viewPort = glm::vec4(0,resx,0,resy);
      update();
      glViewport(0,0,resx,resy);
    }

    // assumes pos.y is inverted in screen coordinates
    glm::vec4 screenToWorld(float x, float y) const
    {
      glm::vec4 ndc(
        2.0*x/resolution.x-1.0,
        2.0*(resolution.y-y)/resolution.y-1.0,
        0.0,
        1.0
      );

      return invProjection*ndc;
    }

    glm::mat4 & getVP(){return vp;}

    glm::mat4 getProjection() const {return projection;}
    void setProjection(glm::mat4 newProjection){projection=newProjection; update();}

    float getZoomLevel() const {return zoomLevel;}

    glm::vec2 getResolution() const {return resolution;}

    glm::vec2 getPosition() const {return position;}

    void setView(glm::mat4 newView){modelView=newView; update();}

    void incrementZoom(float dz)
    {
      if (zoomLevel >= 1.0)
      {
        zoomLevel += dz;
        zoomLevel < 1.0 ? zoomLevel = 1.0 : 0;
      }
      else
      {
        zoomLevel += 1.0/dz;
      }
      update();

    }
    void setPosition(glm::vec2 newPosition){position=newPosition; update();}
    void setPosition(float x, float y){position=glm::vec2(x,y); update();}
    void move(float dx, float dy){position += glm::vec2(dx,dy); update();}

  private:

    void update()
    {
      // scale equally by screen width (all lengths relative to this)
      double maxRes = std::max(resolution.x,resolution.y);
      modelView = glm::scale(glm::mat4(1.0),glm::vec3(maxRes,maxRes,1.0)) *
      // move to position and look at x-y plane from z=1, with up = y axis
        glm::lookAt(
          glm::vec3(position.x,position.y,1.0),
          glm::vec3(position.x,position.y,0.0),
          glm::vec3(0.0,1.0,0.0)
        );

      glm::vec3 center(position.x+0.5,position.y+0.5, 1.0);
      modelView *= glm::translate(glm::mat4(1.0), center) *
            glm::scale(glm::mat4(1.0),glm::vec3(zoomLevel,zoomLevel,1.0))*
            glm::translate(glm::mat4(1.0), -center);

      // finally, project to the screen (ndc)
      projection = glm::ortho(
        0.0,
        double(resolution.x),
        0.0,
        double(resolution.y)
      );


      vp = projection*modelView;
      invProjection = glm::inverse(vp);
    }

    glm::vec2 resolution;
    glm::mat4 modelView;
    glm::mat4 projection;
    glm::mat4 invProjection;
    glm::mat4 vp;

    glm::vec4 viewPort;

    float zoomLevel;

    glm::vec2 position;
  };
}

#endif
