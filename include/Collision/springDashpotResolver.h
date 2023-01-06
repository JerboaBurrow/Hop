#ifndef SPRINGDASHPOTRESOLVER_H
#define SPRINGDASHPOTRESOLVER_H

#include <Collision/collisionResolver.h>
#include <cmath>
#include <Object/objectManager.h>

class SpringDashpot : public CollisionResolver {
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
        std::string & oi, std::string & oj,
        uint64_t pi, uint64_t pj,
        ObjectManager * manager
    );

    void handleWorldCollision(
        uint64_t i,
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
