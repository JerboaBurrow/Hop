#ifndef CCOLLIDEABLE_H
#define CCOLLIDEABLE_H

#include <Object/collisionMesh.h>

struct cCollideable {
    bool isGhost;
    CollisionMesh mesh;
};

#endif /* CCOLLIDEABLE_H */
