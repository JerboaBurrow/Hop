#ifndef HOP_UTIL_H
#define HOP_UTIL_H

#include <utility>
#include <string>

namespace Hop::Util
{

    template <class T>
    struct Triple
    {
        Triple(T f, T s, T t)
        : first(t), second(s), third(t)
        {}

        T first;
        T second;
        T third;
    };

    typedef std::pair<double,double> tupled;

}

#endif /* HOP_UTIL_H */
