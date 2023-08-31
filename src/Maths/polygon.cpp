#include <Maths/polygon.h>

namespace Hop::Maths
{
    HAND Polygon::getHandedness()
    {
        double s = 0.0;
        for (unsigned i = 0; i < vertices.size(); i++)
        {
            unsigned n = next(i);
            s += (vertices[n].x-vertices[i].x)*(vertices[n].y+vertices[i].y);
        }
        
        return s > 0.0 ? HAND::RIGHT : HAND::LEFT;
    }
}