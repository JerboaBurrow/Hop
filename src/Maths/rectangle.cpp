#include <Maths/rectangle.h>

namespace Hop::Maths
{
    BoundingBox boundingBox(std::vector<Vertex> v, double r)
    {
        std::vector<double> x(v.size(), 0.0);

        r *= 0.5;

        std::transform(v.begin(), v.end(), x.begin(), [](Vertex v){return v.x;});
        double maxX = *std::max_element(x.begin(), x.end())+r;
        double minX = *std::min_element(x.begin(), x.end())-r;

        std::transform(v.begin(), v.end(), x.begin(), [](Vertex v){return v.y;});
        double maxY = *std::max_element(x.begin(), x.end())+r;
        double minY = *std::min_element(x.begin(), x.end())-r;

        return BoundingBox
        (
            Vertex(minX, minY),
            Vertex(minX, maxY),
            Vertex(maxX, maxY),
            Vertex(maxX, minY),
            Vertex(minX+(maxX-minX)*0.5,minY+(maxY-minY)*0.5)
        );
    }
}