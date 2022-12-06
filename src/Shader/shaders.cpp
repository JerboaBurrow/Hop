#include <Shader/shaders.h>

Shader NULL_SHADER = Shader("","");

bool operator==(const Shader & lhs, const Shader & rhs){
    return lhs.vertex == rhs.vertex && lhs.fragment == rhs.fragment;
}

void Shader::compile(){
    if (compiled){return;}
    program = glCreateProgram();
    compileShader(program,vertex,fragment);
    compiled = true;
}

void Shader::use(){
    if (!compiled){compile();}
    glUseProgram(program);
}

void Shader::set1f(float a, const char * name){
    glUniform1f(
        glGetUniformLocation(program,name),
        a
    );
}

void Shader::set2f(float a, float b, const char * name){
    glUniform2f(
        glGetUniformLocation(program,name),
        a,b
    );
}

void Shader::set3f(float a, float b, float c, const char * name){
    glUniform3f(
        glGetUniformLocation(program,name),
        a,b,c
    );
}

void Shader::set4f(float a, float b, float c, float d, const char * name){
    glUniform4f(
        glGetUniformLocation(program,name),
        a,b,c,d
    );
}

void Shader::set1i(int i, const char * name){
    glUniform1i(
        glGetUniformLocation(program,name),
        i
    );
}

void Shader::setMatrix4x4(glm::mat4 & m, const char * name, bool transpose){
    glUniformMatrix4fv(
        glGetUniformLocation(program,name),
        1, transpose, &m[0][0]
    );
}

void Shaders::add(Shader & s, std::string n){
    if (shaders.find(n) != shaders.end()){return;}
    if (!s.isCompiled()){s.compile();}
    shaders[n] = s;
}

void Shaders::remove(std::string n){
    if (shaders.find(n) != shaders.end()){return;}
    shaders.erase(n);
}

Shader & Shaders::get(std::string n) {
    if (shaders.find(n) == shaders.end()){return NULL_SHADER;}
    return shaders[n];
}