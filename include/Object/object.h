#ifndef OBJECT_H
#define OBJECT_H

#include <Object/id.h>
#include <Object/physicsState.h>
#include <Object/renderState.h>
#include <gl.h>
#include <memory>

const uint32_t MAX_MESH_VERTICES = 128;

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
    : boundingRectangle(x-w/2.0,y-h/2.0,w,h),
      state(x,y,0.0,ghost),
      renderState(x,y,0.0,1.0),
      glInitialised(false),
      ghost(ghost)
    {}
    Id id;

    void drawDebug(glm::mat4 & proj);

    // setters
    virtual void setPosition(double x, double y);
    virtual void setScale(double s);
    void setGhost(bool b){ghost = b;}
    // getters
    size_t getCollisionMeshSize(){return state.mesh.size();}
    CollisionVertex getCollisionVertex(uint8_t i);
    bool isGhost(){return ghost;}

    ~Object(){
        freeGL();
    }

protected:
    PhysicsState state;
    RenderState renderState;

private:

    bool debug;
    bool glInitialised;
    bool ghost;

    GLuint debugBoundsShader, boundsVAO, boundsVBO;
    GLuint debugMeshShader, meshVAO, meshVBO, meshQuadVBO;

    Rectangle boundingRectangle;

    std::unique_ptr<float[]> boundsVertices, meshVertices, meshQuad;

    void initialiseGL();
    void updateRenderState();
    void freeGL();

};

#endif /* OBJECT_H */
