#ifndef CCOLLIDEABLE_H
#define CCOLLIDEABLE_H

#include <Collision/collisionMesh.h>

struct cCollideable {
    CollisionMesh mesh;

    cCollideable(std::vector<CollisionVertex> v)
    : mesh(CollisionMesh(v))
    {}

    cCollideable(){}
};

#endif /* CCOLLIDEABLE_H */
