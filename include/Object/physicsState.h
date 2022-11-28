#ifndef PHYSICSSTATE_H
#define PHYSICSSTATE_H

#include <Object/collisionMesh.h>

struct PhysicsState {
    double x;
    double y;
    double theta;

    double lastX;
    double lastY;
    double lastTheta;

    double scale;

    bool isGhost;

    CollisionMesh mesh;
};

#endif /* PHYSICSSTATE_H */
