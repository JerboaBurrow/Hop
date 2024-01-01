#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <set>

#include <jThread/jThread.h>

#include <Component/componentArray.h>
#include <Component/cPhysics.h>
#include <Component/cCollideable.h>

#include <Collision/collisionResolver.h>
#include <Object/id.h>

#include <World/world.h>
#include <Util/util.h>

namespace Hop::System::Physics
{

    using Hop::Util::tupled;
    using Hop::World::AbstractWorld;

    using Hop::Object::Id;

    using Hop::Object::Component::ComponentArray;
    using Hop::Object::Component::cCollideable;
    using Hop::Object::Component::cPhysics;

    using jThread::ThreadPool;

    class CollisionDetector 
    {

    public:

        CollisionDetector(tupled lx = tupled(0.0,1.0), tupled ly = tupled(0.0,1.0))
        : limX(lx), limY(ly),
          lX(lx.second-lx.first),
          lY(ly.second-ly.first)
        {}

        CollisionDetector(AbstractWorld * world){}

        virtual ~CollisionDetector() = default;

        virtual void handleObjectObjectCollisions(
            ComponentArray<cCollideable> & dataC,
            ComponentArray<cPhysics> & dataP,
            CollisionResolver * resolver,
            std::set<Id>,
            ThreadPool * workers = nullptr
        ) = 0;

        virtual void handleObjectWorldCollisions(
            ComponentArray<cCollideable> & dataC,
            ComponentArray<cPhysics> & dataP,
            CollisionResolver * resolver,
            AbstractWorld * world,
            std::set<Id> objects,
            ThreadPool * workers = nullptr
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

        tupled limX, limY;
        double lX, lY;

    };

}

#endif /* COLLISIONDETECTOR_H */
