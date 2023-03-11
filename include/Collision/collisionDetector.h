#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <Collision/collisionResolver.h>
#include <Object/objectManager.h>
#include <Object/id.h>

#include <World/tileWorld.h>
#include <World/marchingWorld.h>
#include <Util/util.h>

namespace Hop::Object
{
    class ObjectManager;
}

namespace Hop::System::Physics
{

    using Hop::Object::ObjectManager;
    using Hop::Util::tupled;
    using Hop::World::AbstractWorld;

    class CollisionDetector 
    {

    public:

        CollisionDetector(tupled lx = tupled(0.0,1.0), tupled ly = tupled(0.0,1.0))
        : limX(lx), limY(ly),
        lX(limX.second-limX.first),
        lY(limY.second-limY.first)
        {}

        CollisionDetector(AbstractWorld * world){}

        virtual void handleObjectObjectCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            std::set<Id>
        ) = 0;

        virtual void handleObjectWorldCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            AbstractWorld * world,
            std::set<Id> objects
        ) = 0;

        virtual void handleObjectWorldCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            TileWorld * world,
            std::set<Id> objects
        ) = 0;

        virtual void handleObjectWorldCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            MarchingWorld * world,
            std::set<Id> objects
        ) = 0;

        virtual void centreOn(double x, double y)
        {
            limX = tupled(
                x-lX/2.0,
                x+lX/2.0
            );
            limY = tupled(
                y-lY/2.0,
                y+lY/2.0
            );
        }

    protected:

        double lX, lY;
        tupled limX, limY;
    };

}

#endif /* COLLISIONDETECTOR_H */
