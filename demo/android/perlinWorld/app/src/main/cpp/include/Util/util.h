#ifndef UTIL_H
#define UTIL_H

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

    // forces the number x to be rendered in exactly length characters as a string
    std::string fixedLengthNumber(double x, unsigned length);

}

#endif /* UTIL_H */
