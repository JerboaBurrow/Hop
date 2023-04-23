#ifndef DISTANCE_H
#define DISTANCE_H

namespace Hop::Maths
{

    template <class T>
    T pointLineSegmentDistanceSquared(
        T px, T py,
        T ax, T ay,
        T bx, T by,
        T & nx, T & ny
    )
    {

        T rx = bx-ax; T ry = by-ay;

        T length2 = rx*rx+ry*ry;

        T pMINUSaDOTrOVERlength2 = ((px-ax)*rx + (py-ay)*ry)/length2;

        pMINUSaDOTrOVERlength2 = std::max(static_cast<T>(0.0),std::min(static_cast<T>(1.0),pMINUSaDOTrOVERlength2));

        nx = ax + pMINUSaDOTrOVERlength2 * rx;
        ny = ay + pMINUSaDOTrOVERlength2 * ry;

        T dx = px-nx;
        T dy = py-ny;

        return dx*dx+dy*dy;
    }

    template <class T>
    T pointLineSegmentDistanceSquared(
        T px, T py,
        T ax, T ay,
        T bx, T by
    )
    {
        T nx, ny;
        return pointLineSegmentDistanceSquared(px,py,ax,ay,bx,by,nx,ny);
    }

}

#endif /* DISTANCE_H */
