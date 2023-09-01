#ifndef TRIANGULATION
#define TRIANGULATION

#include <vector>
#include <Maths/polygon.h>
#include <Maths/topology.h>
#include <Maths/triangle.h>

namespace Hop::Maths
{

    class Triangulation 
    {

    public:

        Triangulation() = default;

        Triangulation(const Polygon & p);

        size_t size() const {return triangles.size();}

        const std::vector<Triangle> & getTriangles() const { return triangles; }

    private:

        std::vector<Triangle> triangles;

    };
}

#endif /* TRIANGULATION */
