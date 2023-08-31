#ifndef POLYGON
#define POLYGON

#include <Maths/vertex.h>
#include <Util/util.h>

#include <vector>
#include <algorithm>

namespace Hop::Maths 
{

    using Hop::Util::Triple;

    class Triangulation;

    enum class HAND {LEFT, RIGHT, NOTHING};

    class Polygon 
    {

    public:

        friend class Triangulation;

        Polygon(std::vector<Vertex> v)
        : vertices(v), handedness(getHandedness())
        {}

        Polygon()
        : handedness(HAND::NOTHING)
        {}

        Polygon(const Polygon & p)
        {
            vertices = p.vertices;
            handedness = getHandedness();
        }

        size_t length() const { return vertices.size(); }

        Triple<unsigned> centeredTriangle(unsigned i) { return Triple<unsigned> (last(i), i, next(i));}

        HAND getHandedness() const ;

        void reverse() {std::reverse(vertices.begin(), vertices.end());}

        unsigned next(unsigned i) const { return (i+1)%vertices.size();}
        unsigned last(unsigned i) const { unsigned j = i-1; return j < 0 ? vertices.size()+j : j;}

    private:

        std::vector<Vertex> vertices;
        HAND handedness;

    };

}

#endif /* POLYGON */
