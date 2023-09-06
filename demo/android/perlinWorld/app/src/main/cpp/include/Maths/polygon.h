#ifndef POLYGON
#define POLYGON

#include <Maths/vertex.h>
#include <Maths/triangle.h>

#include <vector>
#include <algorithm>

namespace Hop::Maths 
{

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

        Triangle centeredTriangle(unsigned i) const { return Triangle(vertices[last(i)], vertices[i], vertices[next(i)]);}

        HAND angleSign(unsigned i) const 
        {
            Triangle abc = centeredTriangle(i);
            Vertex a = abc.a;
            Vertex b = abc.b;
            Vertex c = abc.c;
            return (b.x-a.x)*(c.y-b.y)-(b.y-a.y)*(c.x-b.x) > 0 ? HAND::LEFT : HAND::RIGHT;
        }

        const std::vector<Vertex> & getVertices() const { return vertices; }
        std::vector<Line> edges() const ;

        HAND getHandedness() const;

        void reverse() {std::reverse(vertices.begin(), vertices.end());}

        unsigned next(unsigned i) const { return (i+1)%vertices.size();}
        unsigned last(unsigned i) const { int j = int(i)-1; return j < 0 ? vertices.size()+j : j;}

    private:

        std::vector<Vertex> vertices;
        HAND handedness;

    };

}

#endif /* POLYGON */
