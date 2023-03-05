#ifndef TYPEUTILS_H
#define TYPEUTILS_H

#include <Text/type.h>
#include <iostream>

namespace Hop::System::Rendering
{
      
    using namespace Hop::GL;

    void loadASCIIGlyphs(const FT_Face & face, GlyphMap & g);

    // forces the number x to be rendered in exactly length characters as a string
    std::string fixedLengthNumber(double x, unsigned length);

}
#endif
