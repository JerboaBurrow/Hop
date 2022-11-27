#ifndef GL_H
#define GL_H

#ifdef ANDROID

#include <GLES3/gl3.h>

#else

#include <GL/glew.h>
#include <GL/gl.h>

#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <exception>
#include <string>

class GLRuntimeException;
// print buffer status errors
GLuint glBufferStatus(const std::string msg = "");
// print gl error codes
GLuint glError(const std::string msg = "");

// compile a gl shader given a program and source code as const char *
void compileShader(GLuint & shaderProgram, const char * vert, const char * frag);


#endif