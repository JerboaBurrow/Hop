#ifndef SHADERS_H
#define SHADERS_H

#include <gl.h>
#include <string>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <exception>
#include <memory>
#include <jLog/jLog.h>

namespace Hop::System::Rendering
{
   
    using namespace Hop::GL;

    class ShaderSourceNotFound: public std::exception 
    {

    public:

        ShaderSourceNotFound(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }

        std::string msg;

    };


    struct Shader 
    {
        const char * vertex;
        const char * fragment;

        Shader(const char * v, const char * f)
        : vertex(v), fragment(f), compiled(false), used(false)
        {}

        Shader()
        : vertex(""),fragment(""),compiled(false),used(false)
        {}

        Shader(std::string path, std::string name);

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

        bool operator==(const Shader & s)
        {
            return this->vertex == s.vertex && this->fragment == s.fragment;
        }

        bool lint(jLog::Log & log);

    private:

        GLuint program;
        bool compiled;
        bool used;

        std::string parseShaderSource(std::ifstream & file);

    };

    class Shaders 
    {

    public:

        Shaders(){}
        
        void defaultShaders(jLog::Log & log);

        void makeShader(
            const char * v, 
            const char * f,
            std::string n,
            jLog::Log & log
        );
        
        void remove(std::string n);
        std::shared_ptr<Shader> get(std::string n);
        void setProjection(glm::mat4 proj);
        
    private:

        std::unordered_map<std::string,std::shared_ptr<Shader>> shaders;
        
    };
}
#include <Shader/marchingQuad.shader>
#include <Shader/object.shader>
#endif /* SHADERS_H */