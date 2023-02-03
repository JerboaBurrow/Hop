#include <World/tileWorld.h>

TileWorld::TileWorld(
    uint64_t s, 
    OrthoCam & c, 
    uint64_t renderRegion, 
    uint64_t dynamicsRegion,
    uint64_t totalRegion,
    Boundary * b    
)
:   World(s,c,renderRegion,dynamicsRegion, b),
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

unsigned TileWorld::getWorldSizeFromFile(std::string filename){
    std::ifstream read(filename);
    if (read.is_open()){

        std::string line;
        std::string delim = ",";
        std::string datum;

        unsigned worldSize;

        std::getline(read,line);
        size_t next = line.find(delim);
        datum = line.substr(0,next);
        return std::stoul(datum);
    }
    else{
        throw MapReadException("could not open map file "+filename);
    }
    return 0;

}

TileWorld::TileWorld(
        uint64_t s, 
        OrthoCam & c, 
        uint64_t renderRegion, 
        uint64_t dynamicsRegion,
        std::string worldFile,
        Boundary * b
):  World(s,c,renderRegion,dynamicsRegion, b),
    periodicX(periodicX), periodicY(periodicY),
    TOTAL_REGION_SIZE(getWorldSizeFromFile(worldFile)),
    WORLD_HALF_SIZE(TOTAL_REGION_SIZE/2)
{

    std::cout << WORLD_HALF_SIZE << "\n";

    worldBuffer = std::make_unique<Tile[]>(TOTAL_REGION_SIZE*TOTAL_REGION_SIZE);

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

    load(worldFile);

    k = 0;
    for (int i = 0; i < DYNAMICS_REGION_SIZE; i++){
        for (int j = 0; j < DYNAMICS_REGION_SIZE; j++){

            int id = static_cast<std::underlying_type<Tile>::type>(worldBuffer[i*TOTAL_REGION_SIZE+j]);
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
}

bool TileWorld::outOfBounds(int ix, int iy){
    if (ix < 0                      ||
        ix >= TOTAL_REGION_SIZE     ||
        iy < 0                      ||
        iy >= TOTAL_REGION_SIZE
    ){
        return true;
    }
    return false;
}

bool TileWorld::pointOutOfBounds(float x, float y){
    int ix, iy;
    worldToTile(x,y,ix,iy);
    return outOfBounds(ix,iy);
}

bool TileWorld::cameraOutOfBounds(float x, float y){
    int ix, iy;
    worldToTile(x,y,ix,iy);
    return outOfBounds(ix,iy) || outOfBounds(ix+RENDER_REGION_SIZE,iy+RENDER_REGION_SIZE);
}

void TileWorld::tileToIdCoord(int ix, int iy, int & i, int & j){
    i = ix;
    j = iy;
}

void TileWorld::worldToTile(float x, float y, int & ix, int & iy){
    ix = int(std::floor(x*float(RENDER_REGION_SIZE)));
    iy = int(std::floor(y*float(RENDER_REGION_SIZE)));
}

void TileWorld::worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s){

    int ix,iy,i,j;
    worldToTile(x,y,ix,iy);
    if (outOfBounds(ix,iy)){
        h = Tile::EMPTY;
        s = 0.0; x0 = 0.0; y0 = 0.0;
        return;
    }
    tileToIdCoord(ix,iy,i,j);

    h = worldBuffer[i*TOTAL_REGION_SIZE+j];

    s = 1.0/float(RENDER_REGION_SIZE);
    x0 = ix*s;
    y0 = iy*s;

}

Tile TileWorld::tileType(int & i, int & j){

    if (i >= 0 && i < TOTAL_REGION_SIZE && j >= 0 && j < TOTAL_REGION_SIZE){
        return worldBuffer[i*TOTAL_REGION_SIZE+j];
    }
    return Tile::EMPTY;

}


void TileWorld::updateRegion(float x, float y){
    int ix, iy;
    worldToTile(x,y,ix,iy);

    if (outOfBounds(ix,iy) || outOfBounds(ix+int(RENDER_REGION_SIZE),iy+int(RENDER_REGION_SIZE))){
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
    
    std::pair<float,float> p = getPos();
    camera.setPosition(p.first,p.second);
}

/*

    Map file structure

    header 1: tiles on along one dimension (integer), name (string), author (string), RLE (bit)
        with RLE = 0, data 2-x: tile type (integer (0-255))
        with RLE = 1, data 2-x: tile type (integer (0-255)), run length (integer)

    tile types are marching square geometries plus a z-axis
        e.g 15 is full with z=0, 8 is top left with z=0
            31 is full with z=1, 24 is top left with z=1
        The empty tiles are (i for all i%16==0)

*/
void TileWorld::save(std::string filename){
}

void TileWorld::load(std::string filename){
    std::ifstream read(filename);
    if (read.is_open()){

        std::string line;
        std::string delim = ",";
        std::string datum;

        unsigned worldSize;

        bool header = true;
        unsigned k = 0;

        for (unsigned i = 0; i < TOTAL_REGION_SIZE*TOTAL_REGION_SIZE; i++){
            worldBuffer[i] = Tile::EMPTY;
        }

        while (std::getline(read,line)){
            if (header){
                header = false;

                size_t next = line.find(delim);
                datum = line.substr(0,next);
                worldSize = std::stoul(datum);

                if (worldSize != TOTAL_REGION_SIZE){
                    throw MapReadException("Reading world file " + filename + " world size " + std::to_string(TOTAL_REGION_SIZE) + " not equal to map file size " + std::to_string(worldSize));
                }
            }
            else{
                worldBuffer[k] = static_cast<Tile>(std::stoi(line));
                k++;
            }
        }
    }
    else{
        throw MapReadException("could not open map file "+filename);
    }
}