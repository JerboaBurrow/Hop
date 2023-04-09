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

}

#endif /* TRIANGLE_H */
