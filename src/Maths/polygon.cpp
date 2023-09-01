#include <Maths/polygon.h>

namespace Hop::Maths
{

    std::vector<Line> Polygon::edges() const
    {
        std::vector<Line> e;

        for (unsigned i = 0; i < vertices.size(); i++)
        {
            e.push_back(Line(vertices[i], vertices[next(i)]));
        }

        return e;
    }

    HAND Polygon::getHandedness() const 
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