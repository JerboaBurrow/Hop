#include <System/sCollision.h>

#include <iostream>
#include <chrono>
using namespace std::chrono;

namespace Hop::System::Physics
{

    void sCollision::update(ObjectManager * m, AbstractWorld * w, double & oot, double & owt)
    {

        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        detector->handleObjectObjectCollisions(
            m,
            resolver.get(),
            objects
        );

        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        detector->handleObjectWorldCollisions(
            m,
            resolver.get(),
            w,
            objects
        );

        high_resolution_clock::time_point t3 = high_resolution_clock::now();

        oot = duration_cast<duration<double>>(t2 - t1).count();
        owt = duration_cast<duration<double>>(t3 - t2).count();

    }

    void sCollision::centreOn(double x, double y)
    {
        detector->centreOn(x,y);
    }

}
