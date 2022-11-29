#include <World/world.h>
#include <iostream>

const char * marchingQuadVertexShader = "#version 330 core\n"
  "layout(location=0) in vec4 a_position;\n"
  "layout(location=1) in vec3 a_offset;\n"
  "layout(location=2) in float a_id;\n"
  "out vec2 texCoord;\n"
  "flat out int id;\n"
  "uniform mat4 proj;\n"
  "void main(){\n"
  " vec4 pos = proj*vec4(a_position.xy*a_offset.z+a_offset.xy,0.0,1.0);\n"
  " gl_Position = pos;\n"
  " id = int(a_id);\n"
  // transposed texs
  " texCoord = a_position.wz;\n"
  "}";

const char * marchingQuadFragmentShader = "#version 330 core\n"
  "in vec2 texCoord;\n"
  "flat in int id;\n"
  "out vec4 colour;\n"
  "void main(){"
    "if (id == 0){discard;}\n"
    "if (id == 1 && texCoord.x+texCoord.y > 0.5) {discard;}"
    "if (id == 2 && (1.0-texCoord.x)+texCoord.y > 0.5) {discard;}"
    "if (id == 3 && texCoord.y > 0.5) {discard;}"
    "if (id == 4 && texCoord.x+(texCoord.y-1.0)<0.5) {discard;}"
    "if (id == 5 && (texCoord.x+(1.0-texCoord.y)<0.5 || (1.0-texCoord.x)+texCoord.y < 0.5)) {discard;}"
    "if (id == 6 && texCoord.x < 0.5) {discard;}"
    "if (id == 7 && texCoord.x + (1.0-texCoord.y) < 0.5) {discard;}"
    "if (id == 8 && texCoord.x + (1.0-texCoord.y) > 0.5) {discard;}"
    "if (id == 9 && texCoord.x > 0.5) {discard;}"
    "if (id == 10 && ( ( (1.0-texCoord.x)+(1.0-texCoord.y) < 0.5) || (texCoord.x+texCoord.y<0.5) )) {discard;}"
    "if (id == 11 && (1.0-texCoord.x)+(1.0-texCoord.y)<0.5) {discard;}"
    "if (id == 12 && texCoord.y < 0.5) {discard;}"
    "if (id == 13 && (1.0-texCoord.x)+texCoord.y < 0.5) {discard;}"
    "if (id == 14 && texCoord.x+texCoord.y < 0.5) {discard;}"
    "if (id == 15) {true;}"
    "colour=vec4(1.,0.,0.,1.);"
  "\n}";

World::World(uint64_t s, glm::mat4 p)
: perlin(s,0.07,5.0,5.0,256)
{
    seed = s;
    projection = p;
    posX = 0;
    posY = 0;

    renderRegionBuffer = std::make_unique<bool[]>(RENDER_REGION_BUFFER_SIZE*RENDER_REGION_BUFFER_SIZE);
    renderRegionBackBuffer = std::make_unique<bool[]>(RENDER_REGION_BUFFER_SIZE*RENDER_REGION_BUFFER_SIZE);
    renderOffsets = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE*3);
    renderIds = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE);

    for (int i = 0; i < RENDER_REGION_BUFFER_SIZE; i++){
        for (int j = 0; j < RENDER_REGION_BUFFER_SIZE; j++){
            perlin.getAtCoordinate(i,j,THRESHOLD,RENDER_REGION_BUFFER_SIZE,renderRegionBuffer[i*RENDER_REGION_BUFFER_SIZE+j]);
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

    glError("World constructor");
    glBufferStatus("World constructor");
}

void World::processBufferToOffsets(){
    int k = 0;
    float w = 1.0/RENDER_REGION_SIZE;
    for (int i = 0; i < RENDER_REGION_SIZE; i++){
        for (int j = 0; j < RENDER_REGION_SIZE; j++){
            uint8_t ul = renderRegionBuffer[i*RENDER_REGION_BUFFER_SIZE+j+1];
            uint8_t ur = renderRegionBuffer[(i+1)*RENDER_REGION_BUFFER_SIZE+j+1];
            uint8_t lr = renderRegionBuffer[(i+1)*RENDER_REGION_BUFFER_SIZE+j];
            uint8_t ll = renderRegionBuffer[i*RENDER_REGION_BUFFER_SIZE+j];
            uint8_t hash = ll | (lr<<1) | (ur<<2) | (ul<<3);
            // store transposed
            renderOffsets[k*3] = j*w;
            renderOffsets[k*3+1] = i*w;
            renderOffsets[k*3+2] = w;
            renderIds[k] = float(hash);//renderRegionBuffer[k];
            k++;
        }
    }
}

void World::worldToCell(float x, float y, float & ix, float & iy){
    ix = std::floor(x*float(RENDER_REGION_BUFFER_SIZE));
    iy = std::floor(y*float(RENDER_REGION_BUFFER_SIZE));
}

void World::updateRegion(float x, float y){
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

    for (int i = 0; i < RENDER_REGION_BUFFER_SIZE*RENDER_REGION_BUFFER_SIZE; i++){
        renderRegionBackBuffer[i] = renderRegionBuffer[i];
    }

    for (int i = 0; i < RENDER_REGION_BUFFER_SIZE; i++){
        for (int j = 0; j < RENDER_REGION_BUFFER_SIZE; j++){
            int newIx = i+ox;
            int newIy = j+oy;
            if (newIx > 0 && newIx < RENDER_REGION_BUFFER_SIZE && newIy > 0 && newIy < RENDER_REGION_BUFFER_SIZE){
                // alread know the value, just shuffle it over!
                renderRegionBackBuffer[i*RENDER_REGION_BUFFER_SIZE+j] = renderRegionBuffer[newIx*RENDER_REGION_BUFFER_SIZE+newIy];
            }
            else{
                // need to sample new value
                perlin.getAtCoordinate(newIx+posX,newIy+posY,THRESHOLD,RENDER_REGION_BUFFER_SIZE,renderRegionBackBuffer[i*RENDER_REGION_BUFFER_SIZE+j]);
            }
        }
    }

    for (int i = 0; i < RENDER_REGION_BUFFER_SIZE*RENDER_REGION_BUFFER_SIZE; i++){
        renderRegionBuffer[i] = renderRegionBackBuffer[i];
    }

    
    //perlin.getRegion(ix,iy,THRESHOLD,RENDER_REGION_BUFFER_SIZE,renderRegionBuffer.get());
    
    processBufferToOffsets();

    glBindBuffer(GL_ARRAY_BUFFER,VBOid);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float)*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
        renderIds.get()
    );
    posX = ix;
    posY = iy;
}

void World::draw(){
    glBindVertexArray(VAO);
    glUseProgram(shader);
    glDrawArraysInstanced(GL_TRIANGLES,0,6,RENDER_REGION_SIZE*RENDER_REGION_SIZE);
}

TexturedQuad World::getMap(float r, float g, float b){
    r /= 255.0;
    b /= 255.0;
    g /= 255.0;
    std::unique_ptr<float[]> image = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE*3);
    for (int i = 0; i < RENDER_REGION_SIZE*RENDER_REGION_SIZE; i++){
        float val = renderIds[i] > 0 ? 1 : 0;
        image[i*3] = val*r;
        image[i*3+1] = val*g;
        image[i*3+2] = val*b;
    }

    TexturedQuad tQuad(RENDER_REGION_SIZE,std::move(image),projection);

    return tQuad;
}

TexturedQuad World::getLocalRegionMap(){
    uint64_t n = RENDER_REGION_BUFFER_SIZE;
    //uint64_t m = 3*n;
    //uint64_t o = n*n*3+n;
    bool renderRegion[RENDER_REGION_BUFFER_SIZE*RENDER_REGION_BUFFER_SIZE];
    // select central renderRegion from 3x3 grid
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            renderRegion[i*RENDER_REGION_BUFFER_SIZE+j] = renderRegionBuffer[i*n+j];
        }
    }

    std::unique_ptr<float[]> image = std::make_unique<float[]>(RENDER_REGION_BUFFER_SIZE*RENDER_REGION_BUFFER_SIZE*3);

    for (int i = 0; i < RENDER_REGION_BUFFER_SIZE*RENDER_REGION_BUFFER_SIZE; i++){
        image[i*3] = renderRegion[i];
        image[i*3+1] = 0.0f;
        image[i*3+2] = 0.0f;
    }

    TexturedQuad tQuad(RENDER_REGION_BUFFER_SIZE,std::move(image),projection);
    return tQuad;
}

void World::save(std::string filename){
    std::ofstream of(filename+".map");
    if (!of.is_open()){return;}
    int k = 0;
    of << RENDER_REGION_SIZE << "\n";
    for (int i = 0; i < RENDER_REGION_SIZE; i++){
        for (int j = 0; j < RENDER_REGION_SIZE; j++){
            of << renderOffsets[k*3] << ", "
               << renderOffsets[k*3+1] << ", "
               << renderOffsets[k*3+2] << ", "
               << renderRegionBuffer[k] << "\n";
            k++;
        }
    }
    of.close();
}