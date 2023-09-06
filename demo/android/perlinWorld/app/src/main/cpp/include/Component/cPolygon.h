#ifndef CPOLYGON
#define CPOLYGON

#include <Maths/polygon.h>
#include <Maths/triangulation.h>

namespace Hop::Object::Component
{

    struct cPolygon
    {
        Hop::Maths::Polygon p;
        Hop::Maths::Triangulation t;
    };

}

#endif /* CPOLYGON */
