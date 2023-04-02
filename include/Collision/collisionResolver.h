#ifndef COLLISIONRESOLVER_H
#define COLLISIONRESOLVER_H

#include <string>
#include <Object/id.h>

#include <World/tileWorld.h>
#include <World/marchingWorld.h>

namespace Hop::Object
{
    class ObjectManager;
}

namespace Hop::World 
{
    class world;
}

namespace Hop::System::Physics
{
        
    using Hop::Object::ObjectManager;
    using Hop::Object::Id;

    using Hop::World::TileWorld;
    using Hop::World::MarchingWorld;
    using Hop::World::Tile;
    using Hop::World::TileBoundsData;
    using Hop::World::TileNeighbourData;

    class CollisionResolver 
    {

    public:

        CollisionResolver(){}

        virtual ~CollisionResolver() = default;
        
        virtual void handleObjectObjectCollision(
            Id & objectI, uint64_t particleI,
            Id & objectJ, uint64_t particleJ,
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

}

#endif /* COLLISIONRESOLVER_H */
