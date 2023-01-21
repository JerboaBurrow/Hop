#include <World/fixedWorld.h>

FixedWorld::FixedWorld(
    uint64_t s, 
    glm::mat4 p, 
    uint64_t renderRegion, 
    uint64_t dynamicsRegion,
    uint64_t totalRegion,
    uint64_t periodicX,
    uint64_t periodicY
)
:   World(s,p,renderRegion,dynamicsRegion),
    periodicX(periodicX), periodicY(periodicY),
    TOTAL_REGION_SIZE(totalRegion),
    WORLD_HALF_SIZE(TOTAL_REGION_SIZE/2)
{

    worldBuffer = std::make_unique<Tile[]>(totalRegion*totalRegion);

    float w = 1.0 / RENDER_REGION_SIZE;
    int k = 0;
    for (int i = RENDER_REGION_SIZE; i < RENDER_REGION_SIZE*2; i++){
        for (int j = RENDER_REGION_SIZE; j < RENDER_REGION_SIZE*2; j++){
            renderOffsets[k*3] = (i-RENDER_REGION_SIZE)*w;
            renderOffsets[k*3+1] = (j-RENDER_REGION_SIZE)*w;
            renderOffsets[k*3+2] = w;
            k++;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER,VBOoffset);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        sizeof(float)*3*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
        renderOffsets.get()
    );

    glBindBuffer(GL_ARRAY_BUFFER,0);

}

bool FixedWorld::outOfBounds(int ix, int iy){
    if (ix < -WORLD_HALF_SIZE ||
        ix > WORLD_HALF_SIZE  ||
        iy < -WORLD_HALF_SIZE ||
        iy > WORLD_HALF_SIZE
    ){
        return true;
    }
    return false;
}

void FixedWorld::tileToIdCoord(int ix, int iy, int & i, int & j){
    int ox = -WORLD_HALF_SIZE;
    int oy = -WORLD_HALF_SIZE;

    i = ix - ox;
    j = iy - oy;
}

void FixedWorld::worldToTile(float x, float y, int & ix, int & iy){
    ix = int(std::floor(x*float(RENDER_REGION_SIZE)));
    iy = int(std::floor(y*float(RENDER_REGION_SIZE)));
}

void FixedWorld::worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s){

    int ix,iy,i,j;
    worldToTile(x,y,ix,iy);
    
    if (outOfBounds(ix,iy)){
        h = Tile::NULL_TILE;
        s = 0.0; x0 = 0.0; y0 = 0.0;
        return;
    }

    tileToIdCoord(ix,iy,i,j);

    h = worldBuffer[i*TOTAL_REGION_SIZE+j];

    s = 1.0/float(RENDER_REGION_SIZE);
    x0 = ix*s;
    y0 = iy*s;

}

Tile FixedWorld::tileType(int & i, int & j){

    if (i >= 0 && i < TOTAL_REGION_SIZE && j >= 0 && j < TOTAL_REGION_SIZE){
        return worldBuffer[i*TOTAL_REGION_SIZE+j];
    }
    return Tile::NULL_TILE;

}


void FixedWorld::updateRegion(float x, float y){
    int ix, iy;
    worldToTile(x,y,ix,iy);

    if (outOfBounds(ix,iy)){
        return;
    }

    if (ix==tilePosX && iy == tilePosY){
        return;
    }

    int wi, wj;
    int k = 0;
    for (int i = 0; i < DYNAMICS_REGION_SIZE; i++){
        for (int j = 0; j < DYNAMICS_REGION_SIZE; j++){

            tileToIdCoord(ix+i,iy+j,wi,wj);
            int id = static_cast<std::underlying_type<Tile>::type>(worldBuffer[wi*TOTAL_REGION_SIZE+wj]);
            renderIds[k] = float(id);
            k++;

        }
    }

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

void FixedWorld::save(std::string filename){}

void FixedWorld::load(std::string filename){}