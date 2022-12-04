#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <Object/collisionResolver.h>
#include <Object/objectManager.h>

class ObjectManager;

class CollisionDetector {
public:
    CollisionDetector()
    {}

    virtual void handleObjectCollisions(
        ObjectManager * manager,
        CollisionResolver * resolver
    ) = 0;

    virtual void handleWorldCollisions(
        ObjectManager * manager,
        CollisionResolver * resolver,
        World * world
    ) = 0;


};

#endif /* COLLISIONDETECTOR_H */
