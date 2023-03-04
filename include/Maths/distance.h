#ifndef DISTANCE_H
#define DISTANCE_H

template <class T>
T pointLineSegmentDistanceSquared(
    T px, T py,
    T ax, T ay,
    T bx, T by
)
{

    T rx = bx-ax; T ry = by-ay;

    T length2 = rx*rx+ry*ry;

    T pMINUSaDOTrOVERlength2 = ((px-ax)*rx + (py-ay)*ry)/length2;

    pMINUSaDOTrOVERlength2 = std::max(static_cast<T>(0.0),std::min(static_cast<T>(1.0),pMINUSaDOTrOVERlength2));

    T tx = ax + pMINUSaDOTrOVERlength2 * rx;
    T ty = ay + pMINUSaDOTrOVERlength2 * ry;

    T dx = px-tx;
    T dy = py-ty;

    return dx*dx+dy*dy;
}

#endif /* DISTANCE_H */
