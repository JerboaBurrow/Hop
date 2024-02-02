#ifndef WORLD_H
#define WORLD_H

#include <jGL/OpenGL/gl.h>
#include <random>
#include <string>
#include <fstream>
#include <memory>

#include <jGL/OpenGL/Shader/glShader.h>

#include <Shader/marchingQuad.shader>
#include <World/tile.h>

#include <exception>

#include <jGL/orthoCam.h>

#include <World/boundary.h>
#include <World/mapSource.h>
#include <World/fixedSource.h>

#include <Util/util.h>

#include <Console/lua.h>

namespace Hop::System::Physics
{
    class CollisionDetector;
}

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

        bool operator !=(const TileBoundsData & rhs) { return !(*this == rhs); }

        bool operator ==(const TileBoundsData & rhs)
        {
            return wx0 == rhs.wx0 &&
                wy0 == rhs.wy0 &&
                wx1 == rhs.wx1 &&
                wy1 == rhs.wy1 &&
                nx0 == rhs.nx0 &&
                ny0 == rhs.ny0 &&
                nx1 == rhs.nx1 &&
                ny1 == rhs.ny1 &&
                ex0 == rhs.ex0 &&
                ey0 == rhs.ey0 &&
                ex1 == rhs.ex1 &&
                ey1 == rhs.ey1 &&
                sx0 == rhs.sx0 &&
                sy0 == rhs.sy0 &&
                sx1 == rhs.sx1 &&
                sy1 == rhs.sy1 &&
                nw == rhs.nw &&
                ne == rhs.ne &&
                se == rhs.se &&
                sw == rhs.sw;
        }
    };

    struct TileData 
    {
        TileData(Tile h, float x, float y, float l)
        : tileType(h), x(x), y(y), length(l)
        {}
        
        TileData()
        : tileType(Tile::EMPTY), x(0.0), y(0.0), length(0.0)
        {}

        Tile tileType;
        float x;
        float y;
        float length;
    };

    struct TileNeighbourData
    {
        TileData west, northWest, north, northEast, east, southEast, south, southWest;
    };
      
    using Hop::Util::tupled;

    using Hop::System::Physics::CollisionDetector;
    
    using jGL::Shader;
    using jGL::OrthoCam;

    class MapReadException: public std::exception 
    {

    public:

        MapReadException(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }

        std::string msg;
    };

    class MapWriteException: public std::exception 
    {
    public:

        MapWriteException(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }

        std::string msg;
    };

    class AbstractWorld 
    {

    public:

        AbstractWorld(
            uint64_t s, 
            OrthoCam * c, 
            uint64_t renderRegion, 
            uint64_t dynamicsShell,
            MapSource * f,
            Boundary * b
        );

        virtual void draw();

        virtual void save(std::string fileNameWithoutExtension, bool compressed = true){map->save(fileNameWithoutExtension, compressed); forceUpdate = true;}
        virtual void load(std::string fileNameWithoutExtension, bool compressed = true){map->load(fileNameWithoutExtension, compressed); forceUpdate = true;}

        float worldUnitLength(){return 1.0/RENDER_REGION_SIZE;}
        float worldMaxCollisionPrimitiveSize(){return 0.5*worldUnitLength();}

        void worldToTile(float x, float y, int & ix, int & iy);

        TileData getTileData(float x, float y);

        virtual void worldToTileData(double x, double y, Tile & h, double & x0, double & y0, double & s, int & i, int & j) = 0;

        virtual void neighourTileData
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
        ) = 0;
        
        virtual Tile tileType(int i, int j) = 0;

        virtual void tileToIdCoord(int ix, int iy, int & i, int & j) = 0;

        virtual bool pointOutOfBounds(float x, float y);
        virtual bool cameraOutOfBounds(float x, float y);

        virtual bool updateRegion(float x, float y) = 0;

        std::pair<float,float> getPos(){float u = worldUnitLength(); return std::pair<float,float>(u*tilePosX,u*tilePosY);}

        std::pair<float,float> getMapCenter()
        {
            float u = worldUnitLength(); 
            return std::pair<float,float>(u*(tilePosX+RENDER_REGION_SIZE/2.0),u*(tilePosY+RENDER_REGION_SIZE/2.0));
        }

        tupled getWorldWidth(){
            if (dynamicsShell == 0)
            {
                return tupled(0.0,1.0);
            }

            return tupled(-double(dynamicsShell),double(dynamicsShell));
        }

        virtual ~AbstractWorld()
        {
            glDeleteBuffers(1,&VBOquad);
            glDeleteBuffers(1,&VBOoffset);
            glDeleteBuffers(1,&VBOid);
            glDeleteVertexArrays(1,&VAO);
        }

        void setGridWidth(double d){ gridWidth = d; }

        // Lua bindings

        int lua_worldMaxCollisionPrimitiveSize(lua_State * lua)
        {
            lua_pushnumber(lua,worldMaxCollisionPrimitiveSize());
            return 1;
        }

        Boundary * getBoundary() { return boundary; }

    protected:

        uint64_t seed;

        OrthoCam * camera;
        
        const uint64_t RENDER_REGION_SIZE, DYNAMICS_REGION_SIZE;

        const uint64_t dynamicsShell;

        glm::mat4 modelView, projection, invProjection, vp;

        void updateProjection();

        std::unique_ptr<float[]> dynamicsOffsets;
        std::unique_ptr<float[]> dynamicsIds;

        std::unique_ptr<float[]> renderOffsets;
        std::unique_ptr<float[]> renderIds;

        int posX;
        int posY;

        int tilePosX, tilePosY;

        float gridWidth = 0.0f;

        bool forceUpdate;

        GLuint VBOquad, VBOoffset, VBOid, VAO;

        Boundary * boundary;

        MapSource * map;

        std::unique_ptr<Shader> mapShader;

        float quad[6*4] = {
        // positions  / texture coords
        1.0f,  1.0f, 1.0f, 1.0f,   // top right
        1.0f,  0.0f, 1.0f, 0.0f,   // bottom right
        0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
        0.0f,  1.0f, 0.0f, 1.0f,    // top left 
        0.0f,  0.0f, 0.0f, 0.0f,   // bottom left
        1.0f,  1.0f, 1.0f, 1.0f  // top right
        };
        std::default_random_engine generator;
    };

}

#endif /* WORLD_H */
