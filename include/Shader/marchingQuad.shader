#ifndef MARCHINGQUAD_H
#define MARCHINGQUAD_H


namespace Hop::System::Rendering
{
  static const char * marchingQuadVertexShader __attribute__((unused)) = "#version " GLSL_VERSION "\n"
    "precision lowp float;\n precision lowp int;\n"
    "layout(location=0) in vec4 a_position;\n"
    "layout(location=1) in vec3 a_offset;\n"
    "layout(location=2) in float a_id;\n"
    "uniform float u_scale;\n"
    "out vec2 texCoord;\n"
    "flat out int id;\n"
    "uniform mat4 proj;\n"
    "void main()\n"
    "{\n"
    " vec4 pos = proj*vec4(a_position.xy*a_offset.z*u_scale+a_offset.xy,0.0,1.0);\n"
    " gl_Position = pos;\n"
    " id = int(a_id);\n"
    // transposed texs
    " texCoord = a_position.zw;\n"
    "}";

  static const char * marchingQuadFragmentShader __attribute__((unused)) = "#version " GLSL_VERSION "\n"
    "precision lowp float;\n precision lowp int;\n"
    "in vec2 texCoord;\n"
    "flat in int id;\n"
    "uniform vec4 u_background;\n"
    "uniform vec4 u_foreground;\n"
    "uniform float gridWidth;\n"
    "out vec4 colour;\n"
    "void background(){colour = u_background;}\n"
    "void main()\n"
      "{\n"
      "colour=u_foreground;"
      "if (id == 0){background();}\n"
      "if (id == 1 && texCoord.x+texCoord.y > 0.5) {background();}"
      "if (id == 2 && (1.0-texCoord.x)+texCoord.y > 0.5) {background();}"
      "if (id == 3 && texCoord.y > 0.5) {background();}"
      "if (id == 4 && texCoord.x+(texCoord.y-1.0)<0.5) {background();}"
      "if (id == 5 && (texCoord.x+(1.0-texCoord.y)<0.5 || (1.0-texCoord.x)+texCoord.y < 0.5)) {background();}"
      "if (id == 6 && texCoord.x < 0.5) {background();}"
      "if (id == 7 && texCoord.x + (1.0-texCoord.y) < 0.5) {background();}"
      "if (id == 8 && texCoord.x + (1.0-texCoord.y) > 0.5) {background();}"
      "if (id == 9 && texCoord.x > 0.5) {background();}"
      "if (id == 10 && ( ( (1.0-texCoord.x)+(1.0-texCoord.y) < 0.5) || (texCoord.x+texCoord.y<0.5) )) {background();}"
      "if (id == 11 && (1.0-texCoord.x)+(1.0-texCoord.y)<0.5) {background();}"
      "if (id == 12 && texCoord.y < 0.5) {background();}"
      "if (id == 13 && (1.0-texCoord.x)+texCoord.y < 0.5) {background();}"
      "if (id == 14 && texCoord.x+texCoord.y < 0.5) {background();}"
      "if (id == 15) {true;}"
      "if (gridWidth > 0.0)"
      "{if (texCoord.x <gridWidth || texCoord.y < gridWidth || texCoord.x > (1.0-gridWidth) || texCoord.y > (1.0-gridWidth)){colour=vec4(0.,0.,0.,1.);}}"
    "\n}";
}

  
#endif /* MARCHINGQUAD_H */
