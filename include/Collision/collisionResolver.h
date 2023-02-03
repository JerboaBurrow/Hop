#ifndef COLLISIONRESOLVER_H
#define COLLISIONRESOLVER_H

#include <string>
#include <Object/id.h>

class ObjectManager;
class World;

class CollisionResolver {
public:
    CollisionResolver()
    {}

    // takes two object indices oi and oj
    //  and two object collision vertex ids
    //  pi and pj
    //  handles collision between pi and pj
    //  and updates each object via the manager
    virtual void handleObjectCollision(
        std::string & oi, std::string & oj,
        uint64_t pi, uint64_t pj,
        ObjectManager * manager
    ) = 0;

    virtual void handleObjectWorldCollisions(
        Id id,
        ObjectManager * manager,
        World * world
    ) = 0;

};

#endif /* COLLISIONRESOLVER_H */
