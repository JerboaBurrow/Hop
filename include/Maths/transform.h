#ifndef TRANSFORM_H
#define TRANSFORM_H

namespace Hop::Maths
{
    template <class T>
    void rotateClockWise(T & x, T & y, T & cosine, T & sine)
    {
        T xt, yt;
        xt = x;
        yt = y;

        x = xt*cosine + yt*sine;
        y = yt*cosine - xt*sine; 
    }

}

#endif /* TRANSFORM_H */
