#include <Maths/vertex.h>

namespace Hop::Maths
{

    bool operator==(const Line lhs, const Line rhs)
    {
        return lhs.a == rhs.a && lhs.b == rhs.b;
    }

    bool operator==(const Vertex lhs, const Vertex rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool operator!=(const Vertex lhs, const Vertex rhs)
    {
        return !(lhs.x == rhs.x && lhs.y == rhs.y);
    }

    Vertex operator/(Vertex const lhs, double const rhs)
    {
        return Vertex(lhs.x/rhs,lhs.y/rhs);
    }

    Vertex operator*(Vertex const lhs, double const rhs)
    {
        return Vertex(lhs.x*rhs,lhs.y*rhs);
    }

    Vertex operator*(double const lhs, const Vertex rhs) { return rhs * lhs; }

    Vertex operator+(Vertex const lhs, Vertex const rhs)
    {
        return Vertex(lhs.x+rhs.x,lhs.y+rhs.y);
    }

    Vertex & operator+=( Vertex & lhs, Vertex const & rhs )
    {
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }

    Vertex operator-(Vertex const lhs, Vertex const rhs)
    {
        return Vertex(lhs.x-rhs.x,lhs.y-rhs.y);
    }

    double dot(Vertex v, Vertex u)
    {
        return v.x*u.x+v.y*u.y;
    }

    double cross(Vertex v, Vertex u)
    {
        return v.x*u.y - v.y*u.x;
    }

    double norm(Vertex v){
        return std::sqrt(dot(v,v));
    }

    Vertex angleBisectorRay(Vertex a, Vertex b, Vertex c)
    {
        Vertex u = a-b;
        u = u / norm(u);
        Vertex v = c-b;
        v = v / norm(v);
        return u+v;
    }

    double angle(Vertex a, Vertex b, Vertex c)
    {

        Vertex u = a-b;
        Vertex v = c-b;

        return std::acos(dot(u,v) / (norm(u)*norm(v)));
    }

}