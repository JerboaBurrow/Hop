#ifndef VERTEX_H
#define VERTEX_H

#include <cmath>

namespace Hop::Maths
{

    struct Vertex {
        Vertex(double x, double y)
        : x(x), y(y)
        {}
        double x;
        double y;
    };

    bool operator==(const Vertex & lhs, const Vertex & rhs);
    bool operator!=(const Vertex & lhs, const Vertex & rhs) { return !(lhs == rhs); }

    Vertex operator/(Vertex const lhs, double const rhs);

    Vertex operator*(Vertex const lhs, double const rhs);

    Vertex operator+(Vertex const lhs, Vertex const rhs);

    Vertex & operator+=( Vertex & lhs, Vertex const & rhs );

    Vertex operator-(Vertex const lhs, Vertex const rhs);

    double dot(Vertex v, Vertex u);

    double norm(Vertex v);

    Vertex angleBisectorRay(Vertex a, Vertex b, Vertex c);

    double angle(Vertex a, Vertex b, Vertex c);

}

#endif /* VERTEX_H */
