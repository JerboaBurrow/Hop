#ifndef TRIANGULATION
#define TRIANGULATION

#include <vector>
#include <Maths/polygon.h>

#include <Util/util.h>

namespace Hop::Maths
{

    using Hop::Util::Triple;

    class Triangulation 
    {

    public:

        Triangulation() = default;

        Triangulation(const Polygon & p);

    private:

        std::vector<Vertex> vertices;
        std::vector<Triple<unsigned>> triangles;

    };
}

#endif /* TRIANGULATION */
