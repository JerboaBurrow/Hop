#ifndef UTIL_H
#define UTIL_H

#include <utility>
#include <string>

namespace Hop::Util
{
    typedef std::pair<double,double> tupled;

    // forces the number x to be rendered in exactly length characters as a string
    std::string fixedLengthNumber(double x, unsigned length);

}

#endif /* UTIL_H */
