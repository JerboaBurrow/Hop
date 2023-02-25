#ifndef COLLISIONRESOLVER_H
#define COLLISIONRESOLVER_H

#include <string>
#include <Object/id.h>

#include <World/tileWorld.h>
#include <World/marchingWorld.h>


class ObjectManager;
class World;

class CollisionResolver 
{

public:

    CollisionResolver(){}
    
    virtual void handleObjectObjectCollision(
        std::string & objectI, std::string & objectJ,
        uint64_t particleI, uint64_t particleJ,
        ObjectManager * manager
    ) = 0;

    virtual void handleObjectWorldCollisions(
        Id id,
        ObjectManager * manager,
        TileWorld * world
    ) = 0;

    virtual void handleObjectWorldCollisions(
        Id id,
        ObjectManager * manager,
        MarchingWorld * world
    ) = 0;

};

#endif /* COLLISIONRESOLVER_H */
