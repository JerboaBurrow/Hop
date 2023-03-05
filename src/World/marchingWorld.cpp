#include <World/marchingWorld.h>

namespace Hop::World 
{

    MarchingWorld::MarchingWorld(
        uint64_t s, 
        OrthoCam & c, 
        uint64_t renderRegion, 
        uint64_t dynamicsShell,
        MapSource * f,
        Boundary * b
    )
    : World(s,c,renderRegion,dynamicsShell,f,b),
    RENDER_REGION_BUFFER_SIZE(renderRegion+1),
    RENDER_REGION_START(dynamicsShell*renderRegion),
    DYNAMICS_REGION_BUFFER_SIZE(DYNAMICS_REGION_SIZE+1)
    {

        std::cout << RENDER_REGION_SIZE << ", " << RENDER_REGION_START << ", " << DYNAMICS_REGION_SIZE << "\n";

        renderRegionBuffer = std::make_unique<bool[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE);
        renderRegionBackBuffer = std::make_unique<bool[]>(DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE);

        for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE; i++)
        {
            for (int j = 0; j < DYNAMICS_REGION_BUFFER_SIZE; j++)
            {
                renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j] = map->getAtCoordinate(i-int(RENDER_REGION_START),j-int(RENDER_REGION_START)) > 0;
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

    void MarchingWorld::updateRegion(float x, float y)
    {
        int ix, iy;
        worldToTile(x,y,ix,iy);

        if (cameraOutOfBounds(ix,iy))
        {
            return;
        }

        int oy = iy-tilePosY;
        int ox = ix-tilePosX;
        if (!forceUpdate && oy == 0 && ox == 0)
        {
            return;
        }
        
        forceUpdate = false;


        for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE; i++)
        {
            renderRegionBackBuffer[i] = renderRegionBuffer[i];
        }

        for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE; i++)
        {
            for (int j = 0; j < DYNAMICS_REGION_BUFFER_SIZE; j++)
            {
                int newIx = i+ox;
                int newIy = j+oy;
                if (newIx > 0 && newIx < DYNAMICS_REGION_BUFFER_SIZE && newIy > 0 && newIy < DYNAMICS_REGION_BUFFER_SIZE)
                {
                    // alread know the value, just shuffle it over!
                    renderRegionBackBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j] = renderRegionBuffer[newIx*DYNAMICS_REGION_BUFFER_SIZE+newIy];
                }
                else
                {
                    // need to sample new value
                    renderRegionBackBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j] = map->getAtCoordinate(newIx+tilePosX-int(RENDER_REGION_START),newIy+tilePosY-int(RENDER_REGION_START)) > 0;
                }
            }
        }

        for (int i = 0; i < DYNAMICS_REGION_BUFFER_SIZE*DYNAMICS_REGION_BUFFER_SIZE; i++)
        {
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
        
        std::pair<float,float> p = getPos();
        camera.setPosition(p.first,p.second);
    }

    void MarchingWorld::processBufferToOffsets()
    {
        int k = 0;
        float w = 1.0/RENDER_REGION_SIZE;
        unsigned wi = 0;
        unsigned wj = 0;
        for (int i = RENDER_REGION_START; i < RENDER_REGION_START+RENDER_REGION_SIZE; i++)
        {
            wj = 0;
            for (int j = RENDER_REGION_START; j < RENDER_REGION_START+RENDER_REGION_SIZE; j++)
            {
                uint8_t ul = renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j+1];
                uint8_t ur = renderRegionBuffer[(i+1)*DYNAMICS_REGION_BUFFER_SIZE+j+1];
                uint8_t lr = renderRegionBuffer[(i+1)*DYNAMICS_REGION_BUFFER_SIZE+j];
                uint8_t ll = renderRegionBuffer[i*DYNAMICS_REGION_BUFFER_SIZE+j];
                uint8_t hash = ll | (lr<<1) | (ur<<2) | (ul<<3);

                renderOffsets[k*3] = wi*w;
                renderOffsets[k*3+1] = wj*w;
                renderOffsets[k*3+2] = w;
                renderIds[k] = float(hash);
                k++;
                wj++;
            }
            wi++;
        }

        k = 0;
        w = 1.0/RENDER_REGION_SIZE;
        for (int i = 0; i < DYNAMICS_REGION_SIZE; i++)
        {
            for (int j = 0; j < DYNAMICS_REGION_SIZE; j++)
            {
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

    void MarchingWorld::worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s)
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
    
        if (i >= 0 && i < DYNAMICS_REGION_SIZE && j >= 0 && j < DYNAMICS_REGION_SIZE)
        {
            // data is buffered
            int k = i*DYNAMICS_REGION_SIZE+j;
            int t = static_cast<int>(dynamicsIds[k]);
            if (t < 0 || t > MAX_TILE)
            {
                h = Tile::EMPTY;
            }
            else
            {
                h = static_cast<Tile>(t);
            }

            s = 1.0/float(RENDER_REGION_SIZE);
            x0 = ix*s;
            y0 = iy*s;
        }
        else
        {
            // not buffered, so out of dynamics zone, ignore
            h = Tile::EMPTY;
            s = 0.0;
            x0 = 0.0;
            y0 = 0.0;
        }
    }

    Tile MarchingWorld::tileType(int & i, int & j)
    {
        if (i >= 0 && i < DYNAMICS_REGION_SIZE && j >= 0 && j < DYNAMICS_REGION_SIZE)
        {
            // data is buffered
            int k = i*DYNAMICS_REGION_SIZE+j;
            int h = static_cast<int>(dynamicsIds[k]);
            if (h < 0 || h > MAX_TILE)
            {
                return Tile::EMPTY;
            }
            else
            {
                return static_cast<Tile>(h);
            }
        }
        return Tile::EMPTY;
    }

    void MarchingWorld::tileToIdCoord(int ix, int iy, int & i, int & j)
    {
        // make sure render region is always in centre given a dyanmics shell
        int ox = tilePosX-int(dynamicsShell*RENDER_REGION_SIZE);
        int oy = tilePosY-int(dynamicsShell*RENDER_REGION_SIZE);

        i = ix - ox;
        j = iy - oy;
    }

}