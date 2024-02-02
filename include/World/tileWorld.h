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
            OrthoCam * c, 
            uint64_t renderRegion, 
            uint64_t dynamicsShell,
            MapSource * f,
            Boundary * b
        );

        bool updateRegion(float x, float y);
        
        void worldToTileData(double x, double y, Tile & h, double & x0, double & y0, double & s, int & i, int & j);
        void boundsTileData(double x, double y, Tile & h, TileBoundsData & bounds, double & x0, double & y0, double & s);
        void neighourTileData
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
            bool fillTypes = false
        );

        bool westBounds
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
        );

        bool northBounds
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
        );

        bool eastBounds
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
        );

        bool southBounds
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
        );

        Tile tileType(int i, int j);
        void tileToIdCoord(int ix, int iy, int & i, int & j);

    private:

        bool hardOutOfBounds = false;

    };

}

#endif /* TILEWORLD_H */
