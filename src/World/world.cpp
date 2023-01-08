#include <World/world.h>
#include <iostream>

PerlinWorld::PerlinWorld(uint64_t s, glm::mat4 p, uint64_t renderRegion, uint64_t dynamicsRegion)
: World(s), perlin(s,0.07,5.0,5.0,256), 
  RENDER_REGION_SIZE(renderRegion), RENDER_REGION_BUFFER_SIZE(renderRegion+1),
  DYNAMICS_REGION_SIZE(dynamicsRegion), DYNAMICS_REGION_BUFFER_SIZE(dynamicsRegion+1)
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
           
            renderOffsets[k*3] = (i-RENDER_REGION_SIZE)*w;
            renderOffsets[k*3+1] = (j-RENDER_REGION_SIZE)*w;
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
        
            dynamicsOffsets[k*3] = i*w;
            dynamicsOffsets[k*3+1] = j*w;
            dynamicsOffsets[k*3+2] = w;
            dynamicsIds[k] = float(hash);
            k++;
        }
    }
}

void PerlinWorld::worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s){

    int ix,iy,i,j;
    worldToTile(x,y,ix,iy);
    tileToBufferCoord(ix,iy,i,j);

    if (i >= 0 && i < DYNAMICS_REGION_SIZE && j >= 0 && j < DYNAMICS_REGION_SIZE){
        // data is buffered
        int k = i*DYNAMICS_REGION_SIZE+j;
        int t = static_cast<int>(dynamicsIds[k]);
        if (t < 0 || t > MAX_TILE){
            h = Tile::NULL_TILE;
        }
        else{
            h = static_cast<Tile>(t);
        }

        s = 1.0/float(RENDER_REGION_SIZE);
        x0 = i*s;
        y0 = j*s;
    }
    else{
        // not buffered, so out of dynamics zone, ignore
        h = Tile::NULL_TILE;
        s = 0.0;
        x0 = 0.0;
        y0 = 0.0;
    }
}

Tile PerlinWorld::tileType(int & i, int & j){
    if (i >= 0 && i < DYNAMICS_REGION_SIZE && j >= 0 && j < DYNAMICS_REGION_SIZE){
        // data is buffered
        int k = i*DYNAMICS_REGION_SIZE+j;
        int h = static_cast<int>(dynamicsIds[k]);
        if (h < 0 || h > MAX_TILE){
            return Tile::NULL_TILE;
        }
        else{
            return static_cast<Tile>(h);
        }
    }
    return Tile::NULL_TILE;
}

void PerlinWorld::tileToBufferCoord(int & ix, int & iy, int & i, int & j){
    int ox = posX-RENDER_REGION_SIZE;
    int oy = posY-RENDER_REGION_SIZE;

    i = ix - ox;
    j = iy - oy;
}

void PerlinWorld::worldToTile(float x, float y, int & ix, int & iy){
    ix = int(std::floor(x*float(RENDER_REGION_SIZE)));
    iy = int(std::floor(y*float(RENDER_REGION_SIZE)));
}

void PerlinWorld::worldToField(float x, float y, float & ix, float & iy){
    ix = std::floor(x*float(RENDER_REGION_BUFFER_SIZE));
    iy = std::floor(y*float(RENDER_REGION_BUFFER_SIZE));
}

void PerlinWorld::updateRegion(float x, float y){
    float ix, iy;
    worldToField(x,y,ix,iy);
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
    s.setMatrix4x4(projection, "proj");
    s.set1f(1.0f,"u_alpha");
    s.set1f(1.0f,"u_scale");
    s.set1i(0,"u_transparentBackground");
    s.set3f(1.0f,1.0f,1.0f,"u_background");

    glDrawArraysInstanced(GL_TRIANGLES,0,6,RENDER_REGION_SIZE*RENDER_REGION_SIZE);

    glBindVertexArray(0);
    // glBindVertexArray(minimapVAO);

    // s.set1f(1.0f,"u_alpha");
    // s.set1f(1.0f,"u_scale");
    // s.set1i(0,"u_transparentBackground");
    // s.set3f(1.0f,1.0f,1.0f,"u_background");

    // glDrawArraysInstanced(GL_TRIANGLES,0,6,DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE);
    // glBindVertexArray(0);

    glError("World::draw()");
}

// TexturedQuad PerlinWorld::getMap(float r, float g, float b){
//     r /= 255.0;
//     b /= 255.0;
//     g /= 255.0;
//     std::unique_ptr<float[]> image = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE*3);
//     for (int i = 0; i < DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE; i++){
//         float val = dynamicsIds[i] > 0 ? 1 : 0;
//         image[i*3] = val*r;
//         image[i*3+1] = val*g;
//         image[i*3+2] = val*b;
//     }

//     TexturedQuad tQuad(DYNAMICS_REGION_SIZE,std::move(image),projection);

//     return tQuad;
// }

// TexturedQuad PerlinWorld::getLocalRegionMap(){
//     uint64_t n = DYNAMICS_REGION_BUFFER_SIZE;
//     //uint64_t m = 3*n;
//     //uint64_t o = n*n*3+n;
//     bool renderRegion[DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE];
//     // select central renderRegion from 3x3 grid
//     for (int i = 0; i < n; i++){
//         for (int j = 0; j < n; j++){
//             renderRegion[i*DYNAMICS_REGION_BUFFER_SIZE+j] = renderRegionBuffer[i*n+j];
//         }
//     }

//     std::unique_ptr<float[]> image = std::make_unique<float[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE*3);

//     for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE; i++){
//         image[i*3] = renderRegion[i];
//         image[i*3+1] = 0.0f;
//         image[i*3+2] = 0.0f;
//     }

//     TexturedQuad tQuad(DYNAMICS_REGION_BUFFER_SIZE,std::move(image),projection);
//     return tQuad;
// }

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