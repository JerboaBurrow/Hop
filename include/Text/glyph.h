#ifndef GLYPH_H
#define GLYPH_H

#include <gl.h>

namespace Hop::System::Rendering
{
  struct Glyph 
  {

    Glyph(){}

    Glyph(GLuint & u, glm::ivec2 s, glm::ivec2 b, uint64_t o)
    : textureID(u), size(s), bearing(b), offset(o) {}

    GLuint textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint64_t offset;
    
  };
}
#endif