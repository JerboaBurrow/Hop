#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <Maths/vertex.h>
#include <vector>
#include <algorithm>

namespace Hop::Maths
{

    struct Rectangle 
    {

        Rectangle() = default;

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
    
    struct BoundingBox : public Rectangle
    {

        BoundingBox() = default;

        BoundingBox
        (
            Vertex ll, 
            Vertex ul, 
            Vertex ur, 
            Vertex lr,
            Vertex c
        ) 
        : centre(c)
        {
            this->ll = ll;
            this->ul = ul;
            this->ur = ur;
            this->lr = lr;

        }

        Vertex centre;
    };

    BoundingBox boundingBox(std::vector<Vertex> v, double r = 0.0);
}

#endif /* RECTANGLE_H */
