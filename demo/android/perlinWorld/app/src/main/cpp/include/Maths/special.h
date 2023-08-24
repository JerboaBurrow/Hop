#ifndef SPECIAL_H
#define SPECIAL_H

namespace Hop::Maths
{
    template <class T>
    T sgn(T x)
    {
        if (x == static_cast<T>(0)) 
        { 
            return static_cast<T>(0); 
        }
        else if (x < static_cast<T>(0))
        {
            return static_cast<T>(-1);
        }
        else
        {
            return static_cast<T>(1);
        }
    }
}

#endif /* SPECIAL_H */
