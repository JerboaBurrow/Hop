#include <World/tileWorld.h>

namespace Hop::World 
{

    TileWorld::TileWorld(
        uint64_t s, 
        OrthoCam * c, 
        uint64_t renderRegion, 
        uint64_t dynamicsShell,
        MapSource * f,
        Boundary<double> * b    
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

    void TileWorld::worldToTileData(double x, double y, Tile & h, double & x0, double & y0, double & s, int & i, int & j) 
    {
        if (boundary->outOfBounds(x,y))
        {
            h = Tile::EMPTY;
            s = 0.0; x0 = 0.0; y0 = 0.0;
            i = 0; j = 0;
            return;
        }
        int ix,iy;
        worldToTile(x,y,ix,iy);
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
        double & s,
        int & i,
        int & j,
        bool fillTypes
    )
    {
        if (boundary->outOfBounds(x,y))
        {
            s = 0.0; x0 = 0.0; y0 = 0.0;
            h = Tile::EMPTY;
        }

        int ix,iy;
        worldToTile(x,y,ix,iy);


        h = tileType(i,j);

        tileToIdCoord(ix,iy,i,j);

        s =  1.0 / RENDER_REGION_SIZE;
        x0 = ix*s;
        y0 = iy*s;

        nData.west.length = s;
        nData.west.x = (ix-1)*s;
        nData.west.y = iy*s;

        nData.northWest.length = s;
        nData.northWest.x = (ix-1)*s;
        nData.northWest.y = (iy+1)*s;

        nData.north.length = s;
        nData.north.x = ix*s;
        nData.north.y = (iy+1)*s;

        nData.northEast.length = s;
        nData.northEast.x = (ix+1)*s;
        nData.northEast.y = (iy+1)*s;

        nData.east.length = s;
        nData.east.x = (ix+1)*s;
        nData.east.y = iy*s;

        nData.southEast.length = s;
        nData.southEast.x = (ix+1)*s;
        nData.southEast.y = (iy-1)*s;

        nData.south.length = s;
        nData.south.x = ix*s;
        nData.south.y = (iy-1)*s;

        nData.southWest.length = s;
        nData.southWest.x = (ix-1)*s;
        nData.southWest.y = (iy-1)*s;

        if (fillTypes)
        {
            nData.west.tileType = tileType(i-1,j);
            nData.northWest.tileType = tileType(i-1,j+1);
            nData.north.tileType = tileType(i,j+1);
            nData.northEast.tileType = tileType(i+1,j+1);
            nData.east.tileType = tileType(i+1,j);
            nData.southEast.tileType = tileType(i+1,j-1);
            nData.south.tileType = tileType(i,j-1);
            nData.southWest.tileType = tileType(i-1,j-1);
        }
    }

    bool TileWorld::westBounds
    (
        double x, 
        double y, 
        double rc, 
        TileBoundsData & bounds,
        double x0,
        double y0,
        double s,
        int i,
        int j
    )
    {

        double dd = (x0-x);
        dd *= dd;
        if (dd > rc*rc)
        {
            return false;
        }

        // now get the bounding lines data
        
        // WEST
        Tile w = toTile<uint64_t>(map->getAtCoordinate(i-1,j));
        if 
        (
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
        return true;
    }

    bool TileWorld::northBounds
    (
        double x, 
        double y, 
        double rc, 
        TileBoundsData & bounds,
        double x0,
        double y0,
        double s,
        int i,
        int j
    )
    {

        double dd = ((y0+s)-y);
        dd *= dd;
        if (dd > rc*rc)
        {
            return false;
        }

        Tile n = toTile<uint64_t>(map->getAtCoordinate(i,j+1));
        if 
        ( 
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
        return true;
    }

    bool TileWorld::eastBounds
    (
        double x, 
        double y, 
        double rc, 
        TileBoundsData & bounds,
        double x0,
        double y0,
        double s,
        int i,
        int j
    )
    {

        double dd = ((x0+s)-x);
        dd *= dd;
        if (dd > rc*rc)
        {
            return false;
        }

        Tile e = toTile<uint64_t>(map->getAtCoordinate(i+1,j));
        if 
        (
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

        return true;

    }

    bool TileWorld::southBounds
    (
        double x, 
        double y, 
        double rc, 
        TileBoundsData & bounds,
        double x0,
        double y0,
        double s,
        int i,
        int j
    )
    {

        double dd = (y0-y);
        dd *= dd;
        if (dd > rc*rc)
        {
            return false;
        }

        Tile S = toTile<uint64_t>(map->getAtCoordinate(i,j-1));
        if 
        (
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
            S == Tile::RIGHT_HALF   ||
            S == Tile::TOP_RIGHT    ||
            S == Tile::BOTTOM_LEFT_AND_TOP_RIGHT
        )
        {
            // right blocked
            bounds.sx0 = x0+s/2.0; bounds.sy0 = y0;
            bounds.sx1 = x0+s; bounds.sy1 = y0;
        }

        return true;

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
        if (boundary->outOfBounds(x,y))
        {
            h = Tile::EMPTY;
            s = 0.0; x0 = 0.0; y0 = 0.0;
            return;
        }

        int ix,iy,i,j;
        worldToTile(x,y,ix,iy);
        tileToIdCoord(ix,iy,i,j);

        h = toTile<uint64_t>(map->getAtCoordinate(i,j));

        s = 1.0/float(RENDER_REGION_SIZE);
        x0 = ix*s;
        y0 = iy*s;

        westBounds(x,y,s*2.0,bounds,x0,y0,s,i,j);

        Tile nw = toTile<uint64_t>(map->getAtCoordinate(i-1,j+1));
        if 
        (
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


        northBounds(x,y,s*2.0,bounds,x0,y0,s,i,j);

        Tile ne = toTile<uint64_t>(map->getAtCoordinate(i+1,j+1));
        if 
        (
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

        eastBounds(x,y,s*2.0,bounds,x0,y0,s,i,j);

        Tile se = toTile<uint64_t>(map->getAtCoordinate(i-1,j+1));
        if 
        (
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

        southBounds(x,y,s*2.0,bounds,x0,y0,s,i,j);

        Tile sw = toTile<uint64_t>(map->getAtCoordinate(i-1,j-1));
        if 
        (
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