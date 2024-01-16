#ifndef MAIN_H
#define MAIN_H

#include <jGL/jGL.h>
#include <jGL/OpenGL/openGLInstance.h>
#include <jGL/OpenGL/Shader/glShader.h>
#include <jGL/shape.h>

#include <logo.h>
#include <jGL/Display/desktopDisplay.h>
#include <jGL/orthoCam.h>

#include <jLog/jLog.h>

#include <rand.h>
#include <chrono>
using namespace std::chrono;

const int resX = 800;
const int resY = 1000;

uint8_t frameId = 0;
double deltas[60];

std::unique_ptr<jGL::jGLInstance> jGLInstance;

std::string fixedLengthNumber(double x, unsigned length)
{
    std::string d = std::to_string(x);
    std::string dtrunc(length,' ');
    for (unsigned c = 0; c < dtrunc.length(); c++/*ayy lmao*/)
    {

        if (c >= d.length())
        {
            dtrunc[c] = '0';
        }
        else
        {
            dtrunc[c] = d[c];
        }
    }
    return dtrunc;
}

const char * vertexShader = 
    "#version 330\n"
    "precision lowp float;\n"
    "in vec4 a_position;\n"
    "in vec4 a_offset;\n"
    "in vec4 a_colour;\n"
    "uniform mat4 proj;\n"
    "out vec2 texCoord;\n"
    "out vec4 colour;\n"
    "void main(void){"
        "vec2 pos = a_position.xy*a_offset.w;\n"
        "float ct = cos(a_offset.z); float st = sin(a_offset.z);\n"
        "mat2 rot = mat2(ct, -st, st, ct);\n"
        "pos = rot*pos + a_offset.xy;\n"
        "gl_Position = proj*vec4(pos,0.0,1.0);\n"
        "texCoord = a_position.zw;\n"
        "colour = a_colour;\n"
    "}";

const char * fragmentShader = 
    "#version 330\n"
    "precision lowp float;\n"
    "in vec2 texCoord;\n"
    "in vec4 colour;\n"
    "out vec4 fragment;\n"
    "void main(void){\n" 
        "vec2 c = texCoord-vec2(0.5,0.5);\n"
        "if (dot(c,c) > 0.5*0.5) {discard;}\n"
        "fragment = colour;\n"
    "}";

#endif /* MAIN_H */
