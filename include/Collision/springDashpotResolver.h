#ifndef SPRINGDASHPOTRESOLVER_H
#define SPRINGDASHPOTRESOLVER_H

#include <Collision/collisionResolver.h>
#include <cmath>
#include <Object/objectManager.h>

const double WALL_MASS_MULTIPLIER = 10.0;

class SpringDashpot : public CollisionResolver 
{
public:
    SpringDashpot(
        double tc,
        double cor,
        double f
    )
    {
        updateParameters(tc,cor);
        friction = f;
    }

    void handleObjectCollision(
        std::string & objectI, std::string & objectJ,
        uint64_t particleI, uint64_t particleJ,
        ObjectManager * manager
    );

    void handleObjectWorldCollisions(
        Id id,
        ObjectManager * manager,
        World * world
    );
    
    void updateParameters(
        double tc,
        double cor
    );

private:

    double collisionTime, coefficientOfRestitution;

    // pre-calculated collision parameters
    double alpha, beta, friction;
};

#endif /* SPRINGDASHPOTRESOLVER_H */
