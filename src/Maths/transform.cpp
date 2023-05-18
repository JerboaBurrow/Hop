#include <Maths/transform.h>

namespace Hop::Maths
{

    void rotateClockWise(Rectangle * r, double cosine, double sine)
    {
        rotateClockWise<double>(r->llx, r->lly, cosine, sine);
        rotateClockWise<double>(r->ulx, r->uly, cosine, sine);
        rotateClockWise<double>(r->urx, r->ury, cosine, sine);
        rotateClockWise<double>(r->lrx, r->lry, cosine, sine);

        rotateClockWise<double>(r->x,r->y,cosine,sine);

        r->resetAxes();
    }

    void scale(Rectangle * r, double s)
    {
        r->llx *= s;
        r->lly *= s;

        r->ulx *= s;
        r->uly *= s;

        r->urx *= s;
        r->ury *= s;

        r->lrx *= s;
        r->lry *= s;

        r->x *= s;
        r->y *= s;

        r->r *= s;
    }

    void translate(Rectangle * r, double x, double y)
    {
        r->llx += x;
        r->lly += y;

        r->ulx += x;
        r->uly += y;

        r->urx += x;
        r->ury += y;

        r->lrx += x;
        r->lry += y;

        r->x += x;
        r->y += y;
    }

}
