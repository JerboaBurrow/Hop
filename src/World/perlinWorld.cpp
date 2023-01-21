#include <World/perlinWorld.h>

PerlinWorld::PerlinWorld(uint64_t s, glm::mat4 p, uint64_t renderRegion, uint64_t dynamicsRegion)
: World(s,p,renderRegion,dynamicsRegion),
  RENDER_REGION_BUFFER_SIZE(renderRegion+1),
  DYNAMICS_REGION_BUFFER_SIZE(dynamicsRegion+1),
  perlin(seed,0.07,5.0,5.0,256)
{

    renderRegionBuffer = std::make_unique<bool[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE);
    renderRegionBackBuffer = std::make_unique<bool[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE);

    for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE; i++){
        for (int j = 0; j < DYNAMICS_REGION_BUFFER_SIZE; j++){
            perlin.getAtCoordinate(i,j,THRESHOLD,DYNAMICS_REGION_BUFFER_SIZE,renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j]);
        }
    }

    processBufferToOffsets();

    glBindBuffer(GL_ARRAY_BUFFER,VBOid);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float)*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
        renderIds.get()
    );

    glBindBuffer(GL_ARRAY_BUFFER,VBOoffset);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float)*3*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
        renderOffsets.get()
    );

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glError("World constructor");
    glBufferStatus("World constructor");
}

void PerlinWorld::updateRegion(float x, float y){
    int ix, iy;
    worldToTile(x,y,ix,iy);
    int oy = iy-tilePosY;
    int ox = ix-tilePosX;
    if (oy == 0 && ox == 0){
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
                perlin.getAtCoordinate(newIx+tilePosX,newIy+tilePosY,THRESHOLD,DYNAMICS_REGION_BUFFER_SIZE,renderRegionBackBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j]);
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

    tilePosX = ix; tilePosY = iy;
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
    w = 1.0/RENDER_REGION_SIZE;
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

    glError("World::draw()");
}