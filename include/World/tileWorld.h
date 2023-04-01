#ifndef TILEWORLD_H
#define TILEWORLD_H

#include <World/world.h>

namespace Hop::World 
{

    class TileWorld : public AbstractWorld 
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

        bool updateRegion(float x, float y);
        
        void worldToTileData(float x, float y, Tile & h, float & x0, float & y0, float & s);
        void boundsTileData(double x, double y, Tile & h, TileBoundsData & bounds, double & x0, double & y0, double & s);
        void neighourTileData(float x, float y, TileNeighbourData & nData, Tile & h, float & x0, float & y0, float & s);
        
        Tile tileType(int i, int j);
        void tileToIdCoord(int ix, int iy, int & i, int & j);

    private:

    };

}

#endif /* TILEWORLD_H */
