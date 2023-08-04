#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <Maths/triangle.h>
#include <Maths/vertex.h>

#include <Collision/collisionMesh.h>

using Hop::System::Physics::Rectangle;

namespace Hop::Maths
{

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
        Rectangle * r
    )
    {

        if (pointInTriangle
            (
                Vertex(px, py),
                Triangle
                (
                    Vertex(r->llx, r->lly),
                    Vertex(r->ulx, r->uly),
                    Vertex(r->urx, r->ury)
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
                    Vertex(r->llx, r->lly),
                    Vertex(r->urx, r->ury),
                    Vertex(r->lrx, r->lry)
                )
            )
        )
        {
            return true;
        }

        return false;
    }

}
#endif /* TOPOLOGY_H */
