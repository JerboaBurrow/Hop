#include <Object/object.h>

const char * boundsVertexShader = "#version 330 core\n"
    "layout(location=0) in vec2 a_position;\n"
    "uniform mat4 proj;\n"
    "uniform vec4 u_offset;\n"
    "void main(){\n"
    "float c = cos(u_offset.z); float s = sin(u_offset.z);"
    "float x = a_position.x*c + a_position.y*s;"
    "float y = a_position.y*c - a_position.x*s;"
    "gl_Position = proj*vec4(u_offset.w*vec2(x,y)+u_offset.xy,0.0,1.0);\n"
    "}";

const char * boundsFragmentShader = "#version 330 core\n"
    "precision lowp float; uniform vec3 u_colour; uniform float u_alpha;\n"
    "out vec4 colour;\n"
    "void main(void){ colour = vec4(u_colour.rgb,u_alpha); }\n";

void Object::drawDebug(glm::mat4 & proj){
    if (!glInitialised){initialiseGL();}
    updateRenderState();
    glBindVertexArray(boundsVAO);
    glBindBuffer(GL_ARRAY_BUFFER,boundsVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        2*6*sizeof(float),
        boundsVertices.get(),
        GL_DYNAMIC_DRAW
    );
    glUseProgram(debugBoundsShader);
    glUniformMatrix4fv(
        glGetUniformLocation(debugBoundsShader, "proj"),
        1, false, &proj[0][0]
    );
    glUniform4f(
        glGetUniformLocation(debugBoundsShader,"u_offset"),
        renderState.ox,
        renderState.oy,
        renderState.otheta,
        renderState.os
    );

    glDrawArrays(GL_TRIANGLES,0,6);
    glBindVertexArray(0);

}

void Object::updateRenderState(){
    renderState.ox = state.x;
    renderState.oy = state.y;
    renderState.otheta = state.theta;
    renderState.os = state.scale;
}

void Object::initialiseGL(){
    if (glInitialised){return;}
    
    boundsVertices = std::make_unique<float[]>(2*6);

    boundsVertices[0] = boundingRectangle.x;
    boundsVertices[1] = boundingRectangle.y;
    boundsVertices[2] = boundingRectangle.x;
    boundsVertices[3] = boundingRectangle.y+boundingRectangle.h;
    boundsVertices[4] = boundingRectangle.x+boundingRectangle.w;
    boundsVertices[5] = boundingRectangle.y;
    boundsVertices[6] = boundingRectangle.x;
    boundsVertices[7] = boundingRectangle.y+boundingRectangle.h;
    boundsVertices[8] = boundingRectangle.x+boundingRectangle.w;
    boundsVertices[9] = boundingRectangle.y+boundingRectangle.h;
    boundsVertices[10] = boundingRectangle.x+boundingRectangle.w;
    boundsVertices[11] = boundingRectangle.y;

    glGenVertexArrays(1,&boundsVAO);
    glGenBuffers(1,&boundsVBO);
    glGenVertexArrays(1,&meshVAO);
    glGenBuffers(1,&meshVBO);
    debugBoundsShader = glCreateProgram();
    debugMeshShader = glCreateProgram();

    glBindVertexArray(boundsVAO);
    glBindBuffer(GL_ARRAY_BUFFER,boundsVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        2*6*sizeof(float),
        boundsVertices.get(),
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        false,
        2*sizeof(float),
        0
    );

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    compileShader(
        debugBoundsShader,
        boundsVertexShader,
        boundsFragmentShader
    );
    glUseProgram(debugBoundsShader);

    uint64_t h = id.hash();
    std::default_random_engine e;
    std::uniform_real_distribution<float> U;
    e.seed(h);

    glUniform3f(
        glGetUniformLocation(debugBoundsShader,"u_colour"),
        U(e),
        U(e),
        U(e)
    );

    glUniform1f(
        glGetUniformLocation(debugBoundsShader,"u_alpha"),
        0.5f
    );

    glUniform4f(
        glGetUniformLocation(debugBoundsShader,"u_offset"),
        renderState.ox,
        renderState.oy,
        renderState.otheta,
        renderState.os
    );

    glInitialised = true;
}

void Object::freeGL(){

    if (!glInitialised){return;}

    glDeleteProgram(debugBoundsShader);
    glDeleteProgram(debugMeshShader);
    glDeleteVertexArrays(1,&boundsVAO);
    glDeleteVertexArrays(1,&meshVAO);
    glDeleteBuffers(1,&boundsVBO);
    glDeleteBuffers(1,&boundsVAO);

    boundsVertices.reset();
    meshVertices.reset();

    glInitialised = false;
}