#ifndef TEXTUREDQUAD_H
#define TEXTUREDQUAD_H

#include <gl.h>
#include <memory>

class TexturedQuad {
public:
    TexturedQuad(uint64_t s, std::unique_ptr<float[]> image, glm::mat4 p);
    void draw(float scale = 1.0, float x = 0.0, float y = 0.0);
    
    ~TexturedQuad(){
        glDeleteProgram(shader);
        glDeleteBuffers(1,&VBO);
        glDeleteVertexArrays(1,&VAO);
        glDeleteTextures(1,&tex);
        delete [] data;
    }

private:
    glm::mat4 projection;
    uint64_t size;
    float * data;
    GLuint tex, shader, VAO, VBO;

    float vertices[6*4] = {
    // positions  / texture coords
     1.0f,  1.0f, 1.0f, 1.0f,   // top right
     1.0f,  0.0f, 1.0f, 0.0f,   // bottom right
     0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
     0.0f,  1.0f, 0.0f, 1.0f,    // top left 
     0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
     1.0f,  1.0f, 1.0f, 1.0f  // top right
    };
};

#endif /* TEXTUREDQUAD_H */
