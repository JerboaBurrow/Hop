#include <Object/object.h>
#include <iostream>

const char * circleVertexShader = "#version 330 core\n"
    "layout(location=0) in vec4 a_position;\n"
    "layout(location=1) in vec3 a_offset;\n"
    "uniform mat4 proj;\n"
    "out vec2 texCoord;\n"
    "void main(){\n"
    "gl_Position = proj*vec4(a_offset.z*a_position.xy+a_offset.xy,0.0,1.0);\n"
    "texCoord = a_position.zw;\n"
    "}";

const char * circleFragmentShader = "#version 330 core\n"
    "uniform vec4 u_colour;\n"
    "in vec2 texCoord;\n"
    "out vec4 colour;\n"
    "void main(void){"
    "   vec2 c = texCoord-vec2(0.5,0.5);\n"
    "   if (dot(c,c) > 0.5*0.5) {discard;}\n"
    "   colour = u_colour;\n"
    "}\n";

const char * boundsVertexShader = "#version 330 core\n"
    "layout(location=0) in vec2 a_position;\n"
    "uniform mat4 proj;\n"
    "uniform vec4 u_offset;\n"
    "void main(){\n"
    "gl_Position = proj*vec4(u_offset.w*a_position.xy+u_offset.xy,0.0,1.0);\n"
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

    glBindVertexArray(meshVAO);
    glUseProgram(debugMeshShader);
    glUniformMatrix4fv(
        glGetUniformLocation(debugMeshShader, "proj"),
        1, false, &proj[0][0]
    );

    for (int i = 0; i < state.mesh.size(); i++){
        meshVertices[i*3] = state.mesh[i].x-state.mesh[i].r/2.0;
        meshVertices[i*3+1] = state.mesh[i].y-state.mesh[i].r/2.0;
        meshVertices[i*3+2] = state.mesh[i].r;
    }

    glBindBuffer(GL_ARRAY_BUFFER,meshVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        3*MAX_MESH_VERTICES*sizeof(float),
        meshVertices.get(),
        GL_STATIC_DRAW
    );

    glError("mesh upddate");

    glDrawArraysInstanced(
        GL_TRIANGLES,
        0,
        6,
        state.mesh.size()
    );
    glError("mesh drawing");

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
    glGenBuffers(1,&meshQuadVBO);
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

    glError("rect drawing");

    // mesh drawing

    meshQuad = std::make_unique<float[]>(4*6);
    meshVertices = std::make_unique<float[]>(MAX_MESH_VERTICES*3);

    for (int i = 0; i < state.mesh.size(); i++){
        meshVertices[i*3] = state.mesh[i].x-state.mesh[i].r/2.0;
        meshVertices[i*3+1] = state.mesh[i].y-state.mesh[i].r/2.0;
        meshVertices[i*3+2] = state.mesh[i].r;
    }

    meshQuad[0] = 1.0f; meshQuad[1] = 1.0f; meshQuad[2] = 1.0f; meshQuad[3] = 1.0f;
    meshQuad[4] = 1.0f; meshQuad[5] = 0.0f; meshQuad[6] = 1.0f; meshQuad[7] = 0.0f;
    meshQuad[8] = 0.0f; meshQuad[9] = 0.0f; meshQuad[10] = 0.0f; meshQuad[11] = 0.0f;
    meshQuad[12] = 0.0f; meshQuad[13] = 1.0f; meshQuad[14] = 0.0f; meshQuad[15] = 1.0f;
    meshQuad[16] = 0.0f; meshQuad[17] = 0.0f; meshQuad[18] = 0.0f; meshQuad[19] = 0.0f;
    meshQuad[20] = 1.0f; meshQuad[21] = 1.0f; meshQuad[22] = 1.0f; meshQuad[23] = 1.0f;

    glBindVertexArray(0);
    glBindVertexArray(meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER,meshQuadVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        4*6*sizeof(float),
        meshQuad.get(),
        GL_DYNAMIC_DRAW
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        false,
        4*sizeof(float),
        0
    );
    glVertexAttribDivisor(0,0);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindBuffer(GL_ARRAY_BUFFER,meshVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        3*MAX_MESH_VERTICES*sizeof(float),
        meshVertices.get(),
        GL_DYNAMIC_DRAW
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        false,
        3*sizeof(float),
        0
    );
    glVertexAttribDivisor(1,1);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    compileShader(debugMeshShader,circleVertexShader,circleFragmentShader);

    glUseProgram(debugMeshShader);

    glUniform4f(
        glGetUniformLocation(debugMeshShader,"u_colour"),
        0.0f,0.0f,1.0f,1.0f
    );
    glError("mesh drawing");

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
    glDeleteBuffers(1,&meshQuadVBO);

    boundsVertices.reset();
    meshVertices.reset();
    meshQuad.reset();

    glInitialised = false;
}

CollisionVertex Object::getCollisionVertex(uint8_t i) { 
    return state.mesh[i];
}

void Object::setPosition(double x, double y){
    state.x = x;
    state.y = y;
    state.lastX = x;
    state.lastY = y;

    state.updateWorldMesh();
}

void Object::setScale(double s){
    state.scale = s;
    state.updateWorldMesh();
}