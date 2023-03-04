#ifndef CCOLLIDEABLE_H
#define CCOLLIDEABLE_H

#include <Collision/collisionMesh.h>

struct cCollideable 
{
    CollisionMesh mesh;

    cCollideable
    (
        std::vector<CollisionVertex> v,
        double x,
        double y, 
        double theta, 
        double scale
    )
    : mesh(CollisionMesh(v,x,y,theta,scale))
    {}

    cCollideable(){}
};

#endif /* CCOLLIDEABLE_H */
