#include <System/sCollision.h>

#include <iostream>
#include <chrono>
using namespace std::chrono;

namespace Hop::System::Physics
{

    void sCollision::update
    (
        EntityComponentSystem * m, 
        AbstractWorld * w,
        ThreadPool * workers
    )
    {

        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        ComponentArray<cCollideable> & dataC = m->getComponentArray<cCollideable>();
        ComponentArray<cPhysics> & dataP = m->getComponentArray<cPhysics>();

        detector->handleObjectObjectCollisions(
            dataC,
            dataP,
            resolver.get(),
            objects,
            workers
        );

        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        detector->handleObjectWorldCollisions(
            dataC,
            dataP,
            resolver.get(),
            w,
            objects,
            workers
        );

        high_resolution_clock::time_point t3 = high_resolution_clock::now();

        // oot = duration_cast<duration<double>>(t2 - t1).count();
        // owt = duration_cast<duration<double>>(t3 - t2).count();

    }

    void sCollision::centreOn(double x, double y)
    {
        detector->centreOn(x,y);
    }

}
