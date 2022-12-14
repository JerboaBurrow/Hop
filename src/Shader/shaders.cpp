#include <Shader/shaders.h>

Shader NULL_SHADER = Shader("","");

Shader::Shader(std::string path, std::string name){
    std::ifstream fileVs(path+name+".vs");
    std::ifstream fileFs(path+name+".fs");
    if (fileVs.is_open() && fileFs.is_open()){
        const char * v = parseShaderSource(fileVs);
        const char * f = parseShaderSource(fileFs);
    }
    else{
        throw ShaderSourceNotFound(" attempting to locate source files .vs and .fs at "+path+name);
    }
}

const char * Shader::parseShaderSource(std::ifstream & file){
    std::string src = "";
    std::string line;
    while (std::getline(file,line)){
        src += line + "\n";
    }
    return src.c_str();
}

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

void Shaders::makeShader(
    const char * v, 
    const char * f,
    std::string n
){
    if (shaders.find(n)!=shaders.end()){
        return;
    }

    shaders[n] = std::make_unique<Shader>(
        v,
        f
    );


}

void Shaders::remove(std::string n){
    if (shaders.find(n) != shaders.end()){return;}
    shaders.erase(n);
}

std::shared_ptr<Shader> Shaders::get(std::string n) {
    return shaders[n];
}

void Shaders::setProjection(glm::mat4 proj){
   for (auto it = shaders.begin(); it != shaders.end(); it++){
        std::shared_ptr<Shader> shader = it->second;
        shader->use();
        shader->setMatrix4x4(proj,"proj");
   }
}