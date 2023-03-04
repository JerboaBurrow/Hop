#include <System/sCollision.h>

#include <iostream>
#include <chrono>
using namespace std::chrono;

namespace Hop::System::Physics
{

    void sCollision::update(ObjectManager * m, World * w)
    {
        detector->handleObjectObjectCollisions(
            m,
            resolver.get(),
            objects
        );

        detector->handleObjectWorldCollisions(
            m,
            resolver.get(),
            w,
            objects
        );
    }

    void sCollision::centreOn(double x, double y)
    {
        detector->centreOn(x,y);
    }

}