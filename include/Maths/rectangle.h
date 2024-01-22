#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <Maths/vertex.h>

namespace Hop::Maths
{

    struct Rectangle 
    {

        Rectangle(Vertex ll, Vertex ul, Vertex ur, Vertex lr, Vertex a1, Vertex a2)
        : ll(ll), ul(ul), ur(ur), lr(lr), axis1(a1), axis2(a2)
        {}

        Vertex ll;
        Vertex ul;
        Vertex ur;
        Vertex lr;

        Vertex axis1;
        Vertex axis2;

    };
}

#endif /* RECTANGLE_H */
