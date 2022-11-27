#ifndef TYPEUTILS_H
#define TYPEUTILS_H

#include <Text/type.h>
#include <iostream>

void loadASCIIGlyphs(const FT_Face & face, GlyphMap & g);

// forces the number x to be rendered in exactly length characters as a string
std::string fixedLengthNumber(double x, unsigned length);
#endif
