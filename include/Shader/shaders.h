#ifndef SHADERS_H
#define SHADERS_H

#include <gl.h>
#include <string>
#include <unordered_map>

struct Shader {
    const char * vertex;
    const char * fragment;
    Shader(const char * v, const char * f)
    : vertex(v), fragment(f), compiled(false), used(false)
    {}
    Shader()
    : vertex(""),fragment(""),compiled(false),used(false)
    {}

    ~Shader(){if(compiled){glDeleteProgram(program);}}

    void compile();
    void use();
    bool isCompiled(){return compiled;}

    void set1f(float a, const char * name);
    void set2f(float a, float b, const char * name);
    void set3f(float a, float b, float c, const char * name);
    void set4f(float a, float b, float c, float d, const char * name);

    void set1i(int i, const char * name);

    void setMatrix4x4(glm::mat4 & m, const char * name, bool transpose = false);

    bool operator==(const Shader & s){
        return this->vertex == s.vertex && this->fragment == s.fragment;
    }

private:
    GLuint program;
    bool compiled;
    bool used;
};

class Shaders {
public:
    Shaders(){}

    void add(Shader s, std::string n);
    void remove(std::string n);
    Shader getShader(std::string n);

private:
    std::unordered_map<std::string,Shader> shaders;
};

#endif /* SHADERS_H */