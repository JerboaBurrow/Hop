#ifndef MARCHINGWORLD_H
#define MARCHINGWORLD_H

#include <World/world.h>

#include <World/perlinSource.h>

namespace Hop::World 
{

    class MarchingWorld : public AbstractWorld 
    {

    public:

        MarchingWorld(
            uint64_t s, 
            OrthoCam * c, 
            uint64_t renderRegion, 
            uint64_t dynamicsShell,
            MapSource * f,
            Boundary<double> * b
        );

        void save(std::string filename){}
        void load(std::string filename){}

        void worldToTileData(double x, double y, Tile & h, double & x0, double & y0, double & s, int & i, int & j);
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
        
        Tile tileType(int i, int j);

        void tileToIdCoord(int ix, int iy, int & i, int & j);
        bool updateRegion(float x, float y);

    private:

        const uint64_t RENDER_REGION_BUFFER_SIZE, RENDER_REGION_START, DYNAMICS_REGION_BUFFER_SIZE;

        std::unique_ptr<bool[]> renderRegionBuffer;
        std::unique_ptr<bool[]> renderRegionBackBuffer;

        void processBufferToOffsets();

    };

}

#endif /* MARCHINGWORLD_H */
