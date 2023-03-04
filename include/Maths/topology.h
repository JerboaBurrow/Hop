#ifndef TOPOLOGY_H
#define TOPOLOGY_H

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

}
#endif /* TOPOLOGY_H */
