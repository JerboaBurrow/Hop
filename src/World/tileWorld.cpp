#include <World/tileWorld.h>

namespace Hop::World 
{

    TileWorld::TileWorld(
        uint64_t s, 
        OrthoCam * c, 
        uint64_t renderRegion, 
        uint64_t dynamicsShell,
        MapSource * f,
        Boundary * b    
    )
    :   AbstractWorld(s,c,renderRegion,dynamicsShell,f,b)
    {

        forceUpdate = true;

        float w = 1.0 / RENDER_REGION_SIZE;
        int k = 0;
        for (unsigned i = RENDER_REGION_SIZE; i < RENDER_REGION_SIZE*2; i++)
        {
            for (unsigned j = RENDER_REGION_SIZE; j < RENDER_REGION_SIZE*2; j++)
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

    void TileWorld::neighourTileData
    (
        double x, 
        double y, 
        TileNeighbourData & nData,
        Tile & h,
        double & x0,
        double & y0,
        double & s
    )
    {
    }

    void TileWorld::boundsTileData(
        double x, 
        double y, 
        Tile & h, 
        TileBoundsData & bounds, 
        double & x0, 
        double & y0, 
        double & s
    )
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

        // now get the bounding lines data
        
        // WEST
        Tile w = toTile<uint64_t>(map->getAtCoordinate(i-1,j));
        if 
        (
            boundary->outOfBounds(ix-1,iy)  || 
            w == Tile::FULL                 ||
            w == Tile::RIGHT_HALF
        )
        {
            // fully blocked
            bounds.wx0 = x0; bounds.wy0 = y0;
            bounds.wx1 = x0; bounds.wy1 = y0+s;
        }
        else if 
        (
            w == Tile::TOP_HALF   ||
            w == Tile::TOP_RIGHT  ||
            w == Tile::BOTTOM_LEFT_AND_TOP_RIGHT
        )
        {
            // top blocked
            bounds.wx0 = x0; bounds.wy0 = y0+s/2.0;
            bounds.wx1 = x0; bounds.wy1 = y0+s;
        }
        else if 
        (
            w == Tile::BOTTOM_HALF ||
            w == Tile::BOTTOM_RIGHT ||
            w == Tile::TOP_LEFT_AND_BOTTOM_RIGHT
        )
        {
            // bottom blocked
            bounds.wx0 = x0; bounds.wy0 = y0;
            bounds.wx1 = x0; bounds.wy1 = y0+s/2.0;
        }

        Tile nw = toTile<uint64_t>(map->getAtCoordinate(i-1,j+1));
        if 
        (
            boundary->outOfBounds(ix-1,iy+1)    ||
            nw == Tile::FULL                    ||
            nw == Tile::BOTTOM_HALF             ||
            nw == Tile::RIGHT_HALF              ||
            nw == Tile::BOTTOM_RIGHT            ||
            nw == Tile::TOP_LEFT_AND_BOTTOM_RIGHT
        )
        {
            // nw corner
            bounds.nw = true;
        }


        // NORTH
        Tile n = toTile<uint64_t>(map->getAtCoordinate(i,j+1));
        if 
        (
            boundary->outOfBounds(ix,iy+1)  || 
            n == Tile::FULL                 ||
            n == Tile::BOTTOM_HALF
        )
        {
            // fully blocked
            bounds.nx0 = x0; bounds.ny0 = y0+s;
            bounds.nx1 = x0+s; bounds.ny1 = y0+s;
        }
        else if 
        (
            n == Tile::LEFT_HALF    ||
            n == Tile::BOTTOM_LEFT  ||
            n == Tile::BOTTOM_LEFT_AND_TOP_RIGHT
        )
        {
            // left blocked
            bounds.nx0 = x0; bounds.ny0 = y0+s;
            bounds.nx1 = x0+s/2.0; bounds.ny1 = y0+s;
        }
        else if 
        (
            n == Tile::LEFT_HALF    ||
            n == Tile::BOTTOM_RIGHT ||
            n == Tile::TOP_LEFT_AND_BOTTOM_RIGHT
        )
        {
            // right blocked
            bounds.nx0 = x0+s/2.0; bounds.ny0 = y0+s;
            bounds.nx1 = x0+s; bounds.ny1 = y0+s;
        }

        Tile ne = toTile<uint64_t>(map->getAtCoordinate(i+1,j+1));
        if 
        (
            boundary->outOfBounds(ix-1,iy-1)    ||
            ne == Tile::FULL                    ||
            ne == Tile::BOTTOM_HALF             ||
            ne == Tile::LEFT_HALF               ||
            ne == Tile::BOTTOM_LEFT             ||
            ne == Tile::BOTTOM_LEFT_AND_TOP_RIGHT
        )
        {
            // ne corner
            bounds.ne = true;
        }

        // EAST
        Tile e = toTile<uint64_t>(map->getAtCoordinate(i+1,j));
        if 
        (
            boundary->outOfBounds(ix+1,iy)  || 
            e == Tile::FULL                 ||
            e == Tile::LEFT_HALF
        )
        {
            // fully blocked
            bounds.ex0 = x0+s; bounds.ey0 = y0;
            bounds.ex1 = x0+s; bounds.ey1 = y0+s;
        }
        else if 
        (
            e == Tile::TOP_HALF     ||
            e == Tile::TOP_LEFT     ||
            e == Tile::TOP_LEFT_AND_BOTTOM_RIGHT

        )
        {
            // top blocked
            bounds.ex0 = x0+s; bounds.ey0 = y0+s;
            bounds.ex1 = x0+s; bounds.ey1 = y0+s/2.0;
        }
        else if 
        (
            e == Tile::BOTTOM_HALF ||
            e == Tile::BOTTOM_LEFT ||
            e == Tile::BOTTOM_LEFT_AND_TOP_RIGHT
        )
        {
            // bottom blocked
            bounds.ex0 = x0+s; bounds.ey0 = y0;
            bounds.ex1 = x0+s; bounds.ey1 = y0+s/2.0;
        }

        Tile se = toTile<uint64_t>(map->getAtCoordinate(i-1,j+1));
        if 
        (
            boundary->outOfBounds(ix-1,iy+1)    ||
            se == Tile::FULL                    ||
            se == Tile::TOP_HALF                ||
            se == Tile::LEFT_HALF               ||
            se == Tile::TOP_LEFT                ||
            se == Tile::TOP_LEFT_AND_BOTTOM_RIGHT
        )
        {
            // se corner
            bounds.se = true;
        }

        // SOUTH
        Tile S = toTile<uint64_t>(map->getAtCoordinate(i,j-1));
        if 
        (
            boundary->outOfBounds(ix,iy-1)  || 
            S == Tile::FULL                 ||
            S == Tile::TOP_HALF
        )
        {
            // fully blocked
            bounds.sx0 = x0; bounds.sy0 = y0;
            bounds.sx1 = x0+s; bounds.sy1 = y0;
        }
        else if 
        (
            S == Tile::LEFT_HALF    ||
            S == Tile::TOP_LEFT     ||
            S == Tile::TOP_LEFT_AND_BOTTOM_RIGHT
        )
        {
            // left blocked
            bounds.sx0 = x0; bounds.sy0 = y0;
            bounds.sx1 = x0+s/2.0; bounds.sy1 = y0;
        }
        else if 
        (
            e == Tile::RIGHT_HALF   ||
            e == Tile::TOP_RIGHT    ||
            e == Tile::BOTTOM_LEFT_AND_TOP_RIGHT
        )
        {
            // right blocked
            bounds.sx0 = x0+s/2.0; bounds.sy0 = y0;
            bounds.sx1 = x0+s; bounds.sy1 = y0;
        }

        Tile sw = toTile<uint64_t>(map->getAtCoordinate(i-1,j-1));
        if 
        (
            boundary->outOfBounds(ix-1,iy-1)    ||
            sw == Tile::FULL                    ||
            sw == Tile::TOP_HALF                ||
            sw == Tile::RIGHT_HALF              ||
            sw == Tile::TOP_RIGHT               ||
            sw == Tile::BOTTOM_LEFT_AND_TOP_RIGHT
        )
        {
            // sw corner
            bounds.sw = true;
        }
    }

    Tile TileWorld::tileType(int i, int j)
    {

        return toTile<uint64_t>(map->getAtCoordinate(i,j));

    }


    bool TileWorld::updateRegion(float x, float y)
    {
        int ix, iy;
        worldToTile(x,y,ix,iy);

        if (cameraOutOfBounds(ix,iy))
        {
            return false;
        }

        if (!forceUpdate && ix==tilePosX && iy == tilePosY)
        {
            return false;
        }

        forceUpdate = false;

        int k = 0;
        for (unsigned i = 0; i < RENDER_REGION_SIZE; i++)
        {
            for (unsigned j = 0; j < RENDER_REGION_SIZE; j++)
            {


                //tileToIdCoord(ix+i,iy+j,wi,wj);
                uint64_t id = map->getAtCoordinate(i,j); 
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
        camera->setPosition(p.first,p.second);

        return true;
    }

}