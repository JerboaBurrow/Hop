#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <Maths/vertex.h>

namespace Hop::Maths
{

    struct Triangle 
    {

        Triangle(Vertex a, Vertex b, Vertex c)
        : a(a), b(b), c(c) {}
        Vertex a;
        Vertex b;
        Vertex c;

    };

    template <class T>
    T triangleArea
    (
        T ax, T ay,
        T bx, T by,
        T cx, T cy
    )
    {
        T a2, b2, c2, a2x, a2y, b2x, b2y, c2x, c2y;

        a2x = (ax-bx);
        a2y = (ay-by);

        a2 = a2x*a2x+a2y*a2y;

        b2x = (bx-cx);
        b2y = (by-cy);

        b2 = b2x*b2x+b2y*b2y;

        c2x = (cx-ax);
        c2y = (cy-ay);

        c2 = c2x*c2x+c2y*c2y;

        T A = a2+b2+c2;
        A = A * A;

        T B = a2*a2+b2*b2+c2*c2;

        return 0.25 * std::sqrt( A-2.0*B );
    }

}

#endif /* TRIANGLE_H */
