#ifndef PARTICLE_H
#define PARTICLE_H

#include <Object/object.h>
#include <Object/vertex.h>

class Particle : public Object {
public:
    Particle(double x, double y, double r, bool ghost = false)
    : Object(x,y,r,r,ghost)
    {
        state.mesh = CollisionMesh(
            std::vector<CollisionVertex>{CollisionVertex(x,y,r)}
        );
    }

    Vertex getVelocity(double x, double y);
};

#endif /* PARTICLE_H */
