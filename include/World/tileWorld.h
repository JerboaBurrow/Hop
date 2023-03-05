#ifndef TILEWORLD_H
#define TILEWORLD_H

#include <World/world.h>

namespace Hop::World 
{

    /*
        Hold corner and line data for tile boundaries
    
        n
        ____
    w  |    | e
       |____|
        s
    */
    struct TileBoundsData
    {
        TileBoundsData()
        :   wx0(0),wy0(0),wx1(0),wy1(0),nw(false),
            nx0(0),ny0(0),nx1(0),ny1(0),ne(false),
            ex0(0),ey0(0),ex1(0),ey1(0),se(false),
            sx0(0),sy0(0),sx1(0),sy1(0),sw(false)
        {}
        float wx0, wy0, wx1, wy1;
        bool nw;
        float nx0, ny0, nx1, ny1;
        bool ne;
        float ex0, ey0, ex1, ey1;
        bool se;
        float sx0, sy0, sx1, sy1;
        bool sw;
    };

    class TileWorld : public World 
    {
        
    public:

        TileWorld(
            uint64_t s, 
            OrthoCam & c, 
            uint64_t renderRegion, 
            uint64_t dynamicsShell,
            MapSource * f,
            Boundary * b
        );

        void updateRegion(float x, float y);

        void worldToTile(float x, float y, int & ix, int & iy);
        void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s);
        void neighourTileData(double x, double y, Tile & h, TileBoundsData & bounds, double & x0, double & y0, double & s);
        Tile tileType(int & i, int & j);
        void tileToIdCoord(int ix, int iy, int & i, int & j);

    private:

    };

}

#endif /* TILEWORLD_H */
