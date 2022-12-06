#include <World/world.h>
#include <iostream>

const char * marchingQuadVertexShader = "#version 330 core\n"
  "layout(location=0) in vec4 a_position;\n"
  "layout(location=1) in vec3 a_offset;\n"
  "layout(location=2) in float a_id;\n"
  "uniform float u_scale;\n"
  "out vec2 texCoord;\n"
  "flat out int id;\n"
  "uniform mat4 proj;\n"
  "void main(){\n"
  " vec4 pos = proj*vec4(a_position.xy*a_offset.z*u_scale+a_offset.xy,0.0,1.0);\n"
  " gl_Position = pos;\n"
  " id = int(a_id);\n"
  // transposed texs
  " texCoord = a_position.wz;\n"
  "}";

const char * marchingQuadFragmentShader = "#version 330 core\n"
  "in vec2 texCoord;\n"
  "flat in int id;\n"
  "uniform float u_alpha;\n"
  "uniform int u_transparentBackground;\n"
  "uniform vec3 u_background;\n"
  "out vec4 colour;\n"
  "void background(){if(u_transparentBackground==1){discard;}else{colour=vec4(u_background,u_alpha);}}\n"
  "void main(){"
    "colour=vec4(1.,0.,0.,u_alpha);"
    "if (id == 0){background();}\n"
    "if (id == 1 && texCoord.x+texCoord.y > 0.5) {background();}"
    "if (id == 2 && (1.0-texCoord.x)+texCoord.y > 0.5) {background();}"
    "if (id == 3 && texCoord.y > 0.5) {background();}"
    "if (id == 4 && texCoord.x+(texCoord.y-1.0)<0.5) {background();}"
    "if (id == 5 && (texCoord.x+(1.0-texCoord.y)<0.5 || (1.0-texCoord.x)+texCoord.y < 0.5)) {background();}"
    "if (id == 6 && texCoord.x < 0.5) {background();}"
    "if (id == 7 && texCoord.x + (1.0-texCoord.y) < 0.5) {background();}"
    "if (id == 8 && texCoord.x + (1.0-texCoord.y) > 0.5) {background();}"
    "if (id == 9 && texCoord.x > 0.5) {background();}"
    "if (id == 10 && ( ( (1.0-texCoord.x)+(1.0-texCoord.y) < 0.5) || (texCoord.x+texCoord.y<0.5) )) {background();}"
    "if (id == 11 && (1.0-texCoord.x)+(1.0-texCoord.y)<0.5) {background();}"
    "if (id == 12 && texCoord.y < 0.5) {background();}"
    "if (id == 13 && (1.0-texCoord.x)+texCoord.y < 0.5) {background();}"
    "if (id == 14 && texCoord.x+texCoord.y < 0.5) {background();}"
    "if (id == 15) {true;}"
  "\n}";

PerlinWorld::PerlinWorld(uint64_t s, glm::mat4 p)
: World(s), perlin(s,0.07,5.0,5.0,256)
{
    projection = p;
    posX = 0;
    posY = 0;

    renderRegionBuffer = std::make_unique<bool[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE);
    renderRegionBackBuffer = std::make_unique<bool[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE);
    renderOffsets = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE*3);
    renderIds = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE);

    dynamicsOffsets = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE*3);
    dynamicsIds = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE);

    for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE; i++){
        for (int j = 0; j < DYNAMICS_REGION_BUFFER_SIZE; j++){
            perlin.getAtCoordinate(i,j,THRESHOLD,DYNAMICS_REGION_BUFFER_SIZE,renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j]);
        }
    }

    processBufferToOffsets();

    shader = glCreateProgram();
    compileShader(shader,marchingQuadVertexShader,marchingQuadFragmentShader);
    glUseProgram(shader);

    glUniformMatrix4fv(
        glGetUniformLocation(shader,"proj"),
        1,
        GL_FALSE,
        &projection[0][0]
    );

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBOoffset);
    glGenBuffers(1,&VBOquad);
    glGenBuffers(1,&VBOid);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBOquad);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float)*4*6,
        quad,
        GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(float),
        0
    );
    glVertexAttribDivisor(0,0);

    glBindBuffer(GL_ARRAY_BUFFER,VBOoffset);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float)*3*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
        renderOffsets.get(),
        GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        3*sizeof(float),
        0
    );
    glVertexAttribDivisor(1,1);

    glBindBuffer(GL_ARRAY_BUFFER,VBOid);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float)*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
        renderIds.get(),
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float),
        0
    );
    glVertexAttribDivisor(2,1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    // minimap

    glGenVertexArrays(1,&minimapVAO);
    glGenBuffers(1,&minimapVBOoffset);
    glGenBuffers(1,&minimapVBOid);
    glBindVertexArray(minimapVAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBOquad);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float)*4*6,
        quad,
        GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        4,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(float),
        0
    );
    glVertexAttribDivisor(0,0);

    glBindBuffer(GL_ARRAY_BUFFER,minimapVBOoffset);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float)*3*DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE,
        dynamicsOffsets.get(),
        GL_STATIC_DRAW
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        3*sizeof(float),
        0
    );
    glVertexAttribDivisor(1,1);

    glBindBuffer(GL_ARRAY_BUFFER,minimapVBOid);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float)*DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE,
        dynamicsIds.get(),
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        1,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float),
        0
    );
    glVertexAttribDivisor(2,1);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    glError("World constructor");
    glBufferStatus("World constructor");
}

void PerlinWorld::processBufferToOffsets(){
    int k = 0;
    float w = 1.0/RENDER_REGION_SIZE;
    for (int i = RENDER_REGION_SIZE; i < RENDER_REGION_SIZE*2; i++){
        for (int j = RENDER_REGION_SIZE; j < RENDER_REGION_SIZE*2; j++){
            uint8_t ul = renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j+1];
            uint8_t ur = renderRegionBuffer[(i+1)*DYNAMICS_REGION_BUFFER_SIZE+j+1];
            uint8_t lr = renderRegionBuffer[(i+1)*DYNAMICS_REGION_BUFFER_SIZE+j];
            uint8_t ll = renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j];
            uint8_t hash = ll | (lr<<1) | (ur<<2) | (ul<<3);
            // store transposed
            renderOffsets[k*3] = (j-RENDER_REGION_SIZE)*w;
            renderOffsets[k*3+1] = (i-RENDER_REGION_SIZE)*w;
            renderOffsets[k*3+2] = w;
            renderIds[k] = float(hash);
            k++;
        }
    }

    k = 0;
    w = minimapSize*1.0/DYNAMICS_REGION_SIZE;
    for (int i = 0; i < DYNAMICS_REGION_SIZE; i++){
        for (int j = 0; j < DYNAMICS_REGION_SIZE; j++){
            uint8_t ul = renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j+1];
            uint8_t ur = renderRegionBuffer[(i+1)*DYNAMICS_REGION_BUFFER_SIZE+j+1];
            uint8_t lr = renderRegionBuffer[(i+1)*DYNAMICS_REGION_BUFFER_SIZE+j];
            uint8_t ll = renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j];
            uint8_t hash = ll | (lr<<1) | (ur<<2) | (ul<<3);
            // store transposed
            dynamicsOffsets[k*3] = j*w;
            dynamicsOffsets[k*3+1] = i*w;
            dynamicsOffsets[k*3+2] = w;
            dynamicsIds[k] = float(hash);
            k++;
        }
    }
}

void PerlinWorld::worldToCell(float x, float y, float & ix, float & iy){
    ix = std::floor(x*float(RENDER_REGION_BUFFER_SIZE));
    iy = std::floor(y*float(RENDER_REGION_BUFFER_SIZE));
}

void PerlinWorld::updateRegion(float x, float y){
    float ix, iy;
    worldToCell(x,y,ix,iy);
    int tmp = ix;
    ix = iy;
    iy = tmp;
    int oy = iy-posY;
    int ox = ix-posX;
    if (oy == 0 && ox == 0){
        posX = ix;
        posY = iy;
        return;
    }

    for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE; i++){
        renderRegionBackBuffer[i] = renderRegionBuffer[i];
    }

    for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE; i++){
        for (int j = 0; j < DYNAMICS_REGION_BUFFER_SIZE; j++){
            int newIx = i+ox;
            int newIy = j+oy;
            if (newIx > 0 && newIx < DYNAMICS_REGION_BUFFER_SIZE && newIy > 0 && newIy < DYNAMICS_REGION_BUFFER_SIZE){
                // alread know the value, just shuffle it over!
                renderRegionBackBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j] = renderRegionBuffer[newIx*DYNAMICS_REGION_BUFFER_SIZE+newIy];
            }
            else{
                // need to sample new value
                perlin.getAtCoordinate(newIx+posX,newIy+posY,THRESHOLD,DYNAMICS_REGION_BUFFER_SIZE,renderRegionBackBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j]);
            }
        }
    }

    for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE; i++){
        renderRegionBuffer[i] = renderRegionBackBuffer[i];
    }
    
    processBufferToOffsets();

    glBindBuffer(GL_ARRAY_BUFFER,VBOid);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float)*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
        renderIds.get()
    );

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ARRAY_BUFFER,minimapVBOid);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float)*DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE,
        dynamicsIds.get()
    );
    glBindBuffer(GL_ARRAY_BUFFER,0);
    posX = ix;
    posY = iy;
}

void PerlinWorld::draw(Shader & s){
    glBindVertexArray(VAO);
    s.use();
    s.set1f(1.0f,"u_alpha");
    s.set1f(1.0f,"u_scale");
    s.set1i(1,"u_transparentBackground");
    s.set3f(1.0f,1.0f,1.0f,"u_background");

    glDrawArraysInstanced(GL_TRIANGLES,0,6,RENDER_REGION_SIZE*RENDER_REGION_SIZE);

    glBindVertexArray(0);
    glBindVertexArray(minimapVAO);

    s.set1f(1.0f,"u_alpha");
    s.set1f(1.0f,"u_scale");
    s.set1i(0,"u_transparentBackground");
    s.set3f(1.0f,1.0f,1.0f,"u_background");

    glDrawArraysInstanced(GL_TRIANGLES,0,6,DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE);
    glBindVertexArray(0);
}

void PerlinWorld::draw(){
    glBindVertexArray(VAO);
    glUseProgram(shader);
    glUniform1f(
        glGetUniformLocation(shader,"u_alpha"),1.0f
    );
    glUniform1f(
        glGetUniformLocation(shader,"u_scale"),1.0f
    );
    glUniform1i(
        glGetUniformLocation(shader,"u_transparentBackground"),1
    );
    glUniform3f(
        glGetUniformLocation(shader,"u_background"),
        1.0f,1.0f,1.0f
    );
    glDrawArraysInstanced(GL_TRIANGLES,0,6,RENDER_REGION_SIZE*RENDER_REGION_SIZE);

    glBindVertexArray(0);
    glBindVertexArray(minimapVAO);

    glUniform1f(
        glGetUniformLocation(shader,"u_alpha"),1.0f
    );
    glUniform1f(
        glGetUniformLocation(shader,"u_scale"),1.0f
    );
    glUniform1i(
        glGetUniformLocation(shader,"u_transparentBackground"),0
    );
    glUniform3f(
        glGetUniformLocation(shader,"u_background"),
        1.0f,1.0f,1.0f
    );
    glDrawArraysInstanced(GL_TRIANGLES,0,6,DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE);
    glBindVertexArray(0);
}

TexturedQuad PerlinWorld::getMap(float r, float g, float b){
    r /= 255.0;
    b /= 255.0;
    g /= 255.0;
    std::unique_ptr<float[]> image = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE*3);
    for (int i = 0; i < DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE; i++){
        float val = dynamicsIds[i] > 0 ? 1 : 0;
        image[i*3] = val*r;
        image[i*3+1] = val*g;
        image[i*3+2] = val*b;
    }

    TexturedQuad tQuad(DYNAMICS_REGION_SIZE,std::move(image),projection);

    return tQuad;
}

TexturedQuad PerlinWorld::getLocalRegionMap(){
    uint64_t n = DYNAMICS_REGION_BUFFER_SIZE;
    //uint64_t m = 3*n;
    //uint64_t o = n*n*3+n;
    bool renderRegion[DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE];
    // select central renderRegion from 3x3 grid
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            renderRegion[i*DYNAMICS_REGION_BUFFER_SIZE+j] = renderRegionBuffer[i*n+j];
        }
    }

    std::unique_ptr<float[]> image = std::make_unique<float[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE*3);

    for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE; i++){
        image[i*3] = renderRegion[i];
        image[i*3+1] = 0.0f;
        image[i*3+2] = 0.0f;
    }

    TexturedQuad tQuad(DYNAMICS_REGION_BUFFER_SIZE,std::move(image),projection);
    return tQuad;
}

void PerlinWorld::save(std::string filename){
    std::ofstream of(filename+".map");
    if (!of.is_open()){return;}
    int k = 0;
    of << DYNAMICS_REGION_SIZE << "\n";
    for (int i = 0; i < DYNAMICS_REGION_SIZE; i++){
        for (int j = 0; j < DYNAMICS_REGION_SIZE; j++){
            of << renderOffsets[k*3] << ", "
               << renderOffsets[k*3+1] << ", "
               << renderOffsets[k*3+2] << ", "
               << renderRegionBuffer[k] << "\n";
            k++;
        }
    }
    of.close();
}