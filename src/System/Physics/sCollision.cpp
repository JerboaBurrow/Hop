#include <System/Physics/sCollision.h>

namespace Hop::System::Physics
{

    void sCollision::update
    (
        EntityComponentSystem * m, 
        AbstractWorld * w,
        ThreadPool * workers
    )
    {

        ComponentArray<cCollideable> & dataC = m->getComponentArray<cCollideable>();
        ComponentArray<cPhysics> & dataP = m->getComponentArray<cPhysics>();

        detector->handleObjectObjectCollisions(
            dataC,
            dataP,
            resolver.get(),
            objects,
            workers
        );

        detector->handleObjectWorldCollisions(
            dataC,
            dataP,
            resolver.get(),
            w,
            objects,
            workers
        );

    }

    void sCollision::centreOn(double x, double y)
    {
        detector->centreOn(x,y);
    }

}
