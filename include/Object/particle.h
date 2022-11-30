#ifndef PARTICLE_H
#define PARTICLE_H

class Particle : public Object {
public:
    Particle(double x, double y, double r, bool ghost = false)
    : Object(x,y,r,r,ghost)
    {
        state.mesh = CollisionMesh(
            std::vector<CollisionVertex>{CollisionVertex(x,y,r)}
        );
    }
};

#endif /* PARTICLE_H */
