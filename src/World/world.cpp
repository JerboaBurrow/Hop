#include <World/world.h>
#include <iostream>

World::World(uint64_t s, glm::mat4 p, uint64_t renderRegion, uint64_t dynamicsRegion)
: seed(s), projection(p),
  RENDER_REGION_SIZE(renderRegion), 
  DYNAMICS_REGION_SIZE(dynamicsRegion)
{
    tilePosX = 0;
    tilePosY = 0;

    renderOffsets = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE*3);
    renderIds = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE);

    dynamicsOffsets = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE*3);
    dynamicsIds = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE);

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

void World::worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s){

    int ix,iy,i,j;
    worldToTile(x,y,ix,iy);
    tileToIdCoord(ix,iy,i,j);
 
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
        x0 = ix*s;
        y0 = iy*s;
    }
    else{
        // not buffered, so out of dynamics zone, ignore
        h = Tile::NULL_TILE;
        s = 0.0;
        x0 = 0.0;
        y0 = 0.0;
    }
}

Tile World::tileType(int & i, int & j){
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

void World::tileToIdCoord(int & ix, int & iy, int & i, int & j){
    int ox = tilePosX-RENDER_REGION_SIZE;
    int oy = tilePosY-RENDER_REGION_SIZE;

    i = ix - ox;
    j = iy - oy;
}

void World::worldToTile(float x, float y, int & ix, int & iy){
    ix = int(std::floor(x*float(RENDER_REGION_SIZE)));
    iy = int(std::floor(y*float(RENDER_REGION_SIZE)));
}

void World::save(std::string filename){
    // std::ofstream of(filename+".map");
    // if (!of.is_open()){return;}
    // int k = 0;
    // of << DYNAMICS_REGION_SIZE << "\n";
    // for (int i = 0; i < DYNAMICS_REGION_SIZE; i++){
    //     for (int j = 0; j < DYNAMICS_REGION_SIZE; j++){
    //         of << renderOffsets[k*3] << ", "
    //            << renderOffsets[k*3+1] << ", "
    //            << renderOffsets[k*3+2] << ", "
    //            << renderRegionBuffer[k] << "\n";
    //         k++;
    //     }
    // }
    // of.close();
}

void World::load(std::string filename){}

void World::draw(Shader & s){}
