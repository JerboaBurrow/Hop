#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <Collision/collisionResolver.h>
#include <Object/objectManager.h>
#include <Object/id.h>

class ObjectManager;

class CollisionDetector {
public:
    CollisionDetector()
    {}

    virtual void handleObjectCollisions(
        ObjectManager * manager,
        CollisionResolver * resolver,
        std::set<Id>
    ) = 0;

    virtual void handleWorldCollisions(
        ObjectManager * manager,
        CollisionResolver * resolver,
        World * world,
        std::set<Id>
    ) = 0;


};

#endif /* COLLISIONDETECTOR_H */
