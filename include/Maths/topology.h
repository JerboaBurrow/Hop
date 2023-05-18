#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <Maths/triangle.h>
#include <Maths/vertex.h>

#include <Collision/collisionMesh.h>

using Hop::System::Physics::Rectangle;

namespace Hop::Maths
{

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

        T ax = r->ulx-r->llx;
        T ay = r->uly-r->lly;

        T bx = r->urx-r->ulx;
        T by = r->ury-r->uly;

        T a = std::sqrt(ax*ax+ay*ay);
        T b = std::sqrt(bx*bx+by*by);

        T area = a*b;

        T tArea = triangleArea<T>
        (
            px, py,
            r->llx, r->lly,
            r->ulx, r->uly
        );

        tArea += triangleArea<T>
        (
            px, py,
            r->ulx, r->uly,
            r->urx, r->ury
        );

        tArea += triangleArea<T>
        (
            px, py,
            r->urx, r->ury,
            r->lrx, r->lry
        );

        tArea += triangleArea<T>
        (
            px, py,
            r->lrx, r->lry,
            r->llx, r->lly
        );

        if (tArea > area)
        {
            return false;
        }

        return true;

    }

}
#endif /* TOPOLOGY_H */
