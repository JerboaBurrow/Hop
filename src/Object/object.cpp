#include <Object/object.h>
#include <iostream>


namespace Hop::System::Rendering
{
    const char * circleVertexShader = "#version " GLSL_VERSION "\n"
        "in vec4 a_position;\n"
        "in vec3 a_offset;\n"
        "uniform mat4 proj;\n"
        "out vec2 texCoord;\n"
        "void main(){\n"
        "gl_Position = proj*vec4(a_offset.z*a_position.xy+a_offset.xy,0.0,1.0);\n"
        "texCoord = a_position.zw;\n"
        "}";

    const char * circleFragmentShader = "#version " GLSL_VERSION "\n"
        "uniform vec4 u_colour;\n"
        "in vec2 texCoord;\n"
        "out vec4 colour;\n"
        "void main(void){"
        "   vec2 c = texCoord-vec2(0.5,0.5);\n"
        "   if (dot(c,c) > 0.5*0.5) {discard;}\n"
        "   colour = u_colour;\n"
        "}\n";

    const char * boundsVertexShader = "#version " GLSL_VERSION "\n"
        "in vec2 a_position;\n"
        "uniform mat4 proj;\n"
        "uniform vec4 u_offset;\n"
        "void main(){\n"
        "gl_Position = proj*vec4(u_offset.w*a_position.xy+u_offset.xy,0.0,1.0);\n"
        "}";

    const char * boundsFragmentShader = "#version " GLSL_VERSION "\n"
        "precision lowp float; uniform vec3 u_colour; uniform float u_alpha;\n"
        "out vec4 colour;\n"
        "void main(void){ colour = vec4(u_colour.rgb,u_alpha); }\n";
}