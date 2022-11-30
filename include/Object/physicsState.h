#ifndef PHYSICSSTATE_H
#define PHYSICSSTATE_H

#include <Object/collisionMesh.h>

struct PhysicsState {
    PhysicsState()
    : x(0.), y(0.), theta(0.), lastX(0.), lastY(0.), lastTheta(0.),
      scale(1.), isGhost(false)
    {}
    PhysicsState(double x, double y, double theta, bool g)
    : lastX(x), lastY(y), lastTheta(theta), scale(1.), isGhost(g)
    {}
    double x;
    double y;
    double theta;

    double lastX;
    double lastY;
    double lastTheta;

    double scale;

    bool isGhost;

    CollisionMesh mesh;

    void updateWorldMesh();
};

#endif /* PHYSICSSTATE_H */
