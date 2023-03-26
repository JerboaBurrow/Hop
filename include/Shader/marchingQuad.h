#ifndef MARCHINGQUAD_H
#define MARCHINGQUAD_H


namespace Hop::System::Rendering
{
  static const char * marchingQuadVertexShader = "#version " GLSL_VERSION "\n"
    "in vec4 a_position;\n"
    "in vec3 a_offset;\n"
    "in float a_id;\n"
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

  static const char * marchingQuadFragmentShader = "#version " GLSL_VERSION "\n"
    "in vec2 texCoord;\n"
    "flat in int id;\n"
    "uniform float u_alpha;\n"
    "uniform int u_transparentBackground;\n"
    "uniform vec3 u_background;\n"
    "float line = 0.04;\n"
    "vec4 bk;\n"
    "out vec4 colour;\n"
    "void background(){if(u_transparentBackground==1){discard;}else{colour=bk;}}\n"
    "void main()\n"
      "{\n"
      "bk = vec4(u_background,1.0);"
      "colour=vec4(1.,0.,0.,u_alpha);"
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
      //"if (texCoord.x <line || texCoord.y < line || texCoord.x > (1.0-line) || texCoord.y > (1.0-line)){colour=vec4(0.,0.,0.,1.);}"
    "\n}";
}

  
#endif /* MARCHINGQUAD_H */