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

    inline bool endsWith(std::string str, std::string end)
    {
        if (str.size() > end.size())
        {
            if (std::equal(end.rbegin(), end.rend(), str.rbegin()))
            {
                return true;
            }
        }

        return false;
    }

}

#endif /* HOP_UTIL_H */
