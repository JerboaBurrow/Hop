#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <Maths/triangle.h>
#include <Maths/rectangle.h>
#include <Maths/vertex.h>

#include <Maths/polygon.h>

#include <algorithm>

namespace Hop::Maths
{

    Vertex lineLineSegementIntersection(Vertex p, Vertex q, Vertex u, Vertex v);

    bool pointInTriangle(Vertex v, Triangle tri);

    template <class T>
    int pointLineHandedness(
        T x, T y,
        T ax, T ay,
        T bx, T by
    )
    {
        T sx = (bx-ax);
        T sy = (by-ay);
        T tx = (x-ax);
        T ty = (y-ay);

        T d = sx*ty-sy*tx;
        if (d < 0.0)
        {
            return -1;
        }
        else if (d > 0)
        {
            return 1;
        }
        else return 0;
    }

    template <class T>
    bool pointInRectangle
    (
        T px, T py,
        Rectangle r
    )
    {

        if (pointInTriangle
            (
                Vertex(px, py),
                Triangle
                (
                    Vertex(r.ll.x, r.ll.y),
                    Vertex(r.ul.x, r.ul.y),
                    Vertex(r.ur.x, r.ur.y)
                )
            )
        )
        {
            return true;
        }

        if (pointInTriangle
            (
                Vertex(px, py),
                Triangle
                (
                    Vertex(r.ll.x, r.ll.y),
                    Vertex(r.ur.x, r.ur.y),
                    Vertex(r.lr.x, r.lr.y)
                )
            )
        )
        {
            return true;
        }

        return false;
    }

    Vertex findEar(const Polygon & p, const Polygon & q, unsigned i);

    bool isEar(const Polygon & p, unsigned i);

    bool isEar(const Polygon & p, const Vertex & v);

    Line findDiagonal(const Polygon & p, unsigned i);

    bool isGood(const Polygon & p, const Polygon & q);

    Polygon goodSubPolygon(const Polygon & p, const Polygon & q, unsigned i, unsigned j);
}
#endif /* TOPOLOGY_H */
