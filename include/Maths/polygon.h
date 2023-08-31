#ifndef POLYGON
#define POLYGON

#include <Maths/vertex.h>
#include <vector>

namespace Hop::Maths 
{

    enum class HAND {LEFT, RIGHT, NOTHING};

    class Polygon 
    {

    public:

        Polygon(std::vector<Vertex> v)
        : vertices(v), handedness(getHandedness())
        {}

        Polygon()
        : handedness(HAND::NOTHING)
        {}

        HAND getHandedness();

        unsigned next(unsigned i){ return (i+1)%vertices.size();}
        unsigned last(unsigned i){ unsigned j = i-1; return j < 0 ? vertices.size()+j : j;}

    private:

        std::vector<Vertex> vertices;
        HAND handedness;

    };

}

#endif /* POLYGON */
