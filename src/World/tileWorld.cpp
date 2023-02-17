#include <World/tileWorld.h>

TileWorld::TileWorld(
    uint64_t s, 
    OrthoCam & c, 
    uint64_t renderRegion, 
    uint64_t dynamicsShell,
    MapSource * f,
    Boundary * b    
)
:   World(s,c,renderRegion,dynamicsShell,f,b)
{

    forceUpdate = true;

    float w = 1.0 / RENDER_REGION_SIZE;
    int k = 0;
    for (int i = RENDER_REGION_SIZE; i < RENDER_REGION_SIZE*2; i++)
    {
        for (int j = RENDER_REGION_SIZE; j < RENDER_REGION_SIZE*2; j++)
        {
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

void TileWorld::tileToIdCoord(int ix, int iy, int & i, int & j)
{
    i = ix;
    j = iy;
}

void TileWorld::worldToTile(float x, float y, int & ix, int & iy)
{
    ix = int(std::floor(x*float(RENDER_REGION_SIZE)));
    iy = int(std::floor(y*float(RENDER_REGION_SIZE)));
}

void TileWorld::worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s)
{

    int ix,iy,i,j;
    worldToTile(x,y,ix,iy);
    if (boundary->outOfBounds(ix,iy))
    {
        h = Tile::EMPTY;
        s = 0.0; x0 = 0.0; y0 = 0.0;
        return;
    }
    tileToIdCoord(ix,iy,i,j);

    h = toTile<uint64_t>(map->getAtCoordinate(i,j));


    s = 1.0/float(RENDER_REGION_SIZE);
    x0 = ix*s;
    y0 = iy*s;

}

Tile TileWorld::tileType(int & i, int & j)
{

    return toTile<uint64_t>(map->getAtCoordinate(i,j));

}


void TileWorld::updateRegion(float x, float y)
{
    int ix, iy;
    worldToTile(x,y,ix,iy);

    if (cameraOutOfBounds(ix,iy))
    {
        return;
    }

    if (!forceUpdate && ix==tilePosX && iy == tilePosY)
    {
        return;
    }

    forceUpdate = false;

    int wi, wj;
    int k = 0;
    for (int i = 0; i < DYNAMICS_REGION_SIZE; i++)
    {
        for (int j = 0; j < DYNAMICS_REGION_SIZE; j++)
        {

            tileToIdCoord(ix+i,iy+j,wi,wj);
            uint64_t id = map->getAtCoordinate(wi,wj); 
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