#ifndef OBJECT_H
#define OBJECT_H

#include <Object/id.h>
#include <Object/physicsState.h>
#include <Object/renderState.h>
#include <gl.h>
#include <memory>

struct Rectangle{
    Rectangle()
    : x(0.),y(0.),w(0.),h(0.)
    {}

    Rectangle(
        float x, float y, float w, float h
    )
    :x(x),y(y),w(w),h(h) {}

    const float x;
    const float y;
    const float w;
    const float h;
};

class Object {
public:
    Object()
    {}
    Object(float x, float y, float w, float h, bool ghost = false)
    : boundingRectangle(x,y,w,h),
      state(x,y,0.0,ghost),
      renderState(x,y,0.0,1.0)
    {}
    Id id;

    void drawDebug(glm::mat4 & proj);
    ~Object(){
        freeGL();
    }

private:

    bool debug;
    bool glInitialised;

    GLuint debugBoundsShader, boundsVAO, boundsVBO;
    GLuint debugMeshShader, meshVAO, meshVBO;

    Rectangle boundingRectangle;
    PhysicsState state;
    RenderState renderState;

    std::unique_ptr<float[]> boundsVertices, meshVertices;

    void initialiseGL();
    void updateRenderState();
    void freeGL();

};

#endif /* OBJECT_H */
