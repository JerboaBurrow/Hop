#include <World/world.h>

namespace Hop::World 
{

    uint64_t dynamicsRegionSize(uint64_t renderRegion, uint64_t dynamicsShell)
    {

        if (dynamicsShell == 0)
        {
            return renderRegion;
        }
        else
        {
            return (2*dynamicsShell+1) * renderRegion;
        }

    }

    AbstractWorld::AbstractWorld(
        uint64_t s, 
        OrthoCam & c, 
        uint64_t renderRegion, 
        uint64_t dynamicsShell,
        MapSource * f,
        Boundary * b
    )
    : seed(s), camera(c), projection(c.getVP()),
    RENDER_REGION_SIZE(renderRegion), 
    DYNAMICS_REGION_SIZE(dynamicsRegionSize(renderRegion,dynamicsShell)),
    dynamicsShell(dynamicsShell),
    boundary(b), map(f)
    {
        tilePosX = 0;
        tilePosY = 0;

        renderOffsets = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE*3);
        renderIds = std::make_unique<float[]>(RENDER_REGION_SIZE*RENDER_REGION_SIZE);

        dynamicsOffsets = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE*3);
        dynamicsIds = std::make_unique<float[]>(DYNAMICS_REGION_SIZE*DYNAMICS_REGION_SIZE);

        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBOoffset);
        glGenBuffers(1,&VBOquad);
        glGenBuffers(1,&VBOid);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER,VBOquad);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(float)*4*6,
            quad,
            GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            4,
            GL_FLOAT,
            GL_FALSE,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(0,0);

        glBindBuffer(GL_ARRAY_BUFFER,VBOoffset);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(float)*3*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
            renderOffsets.get(),
            GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            3*sizeof(float),
            0
        );
        glVertexAttribDivisor(1,1);

        glBindBuffer(GL_ARRAY_BUFFER,VBOid);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(float)*RENDER_REGION_SIZE*RENDER_REGION_SIZE,
            renderIds.get(),
            GL_DYNAMIC_DRAW
        );

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            1,
            GL_FLOAT,
            GL_FALSE,
            sizeof(float),
            0
        );
        glVertexAttribDivisor(2,1);

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        glError("World constructor");
        glBufferStatus("World constructor");
    }

    void AbstractWorld::draw(Shader & s)
    {
        glBindVertexArray(VAO);
        s.use();
        s.setMatrix4x4(projection, "proj");
        s.set1f(1.0f,"u_alpha");
        s.set1f(1.0f,"u_scale");
        s.set1i(0,"u_transparentBackground");
        s.set3f(1.0f,1.0f,1.0f,"u_background");

        glDrawArraysInstanced(GL_TRIANGLES,0,6,RENDER_REGION_SIZE*RENDER_REGION_SIZE);

        glBindVertexArray(0);

        glError("World::draw()");
    }

    void AbstractWorld::worldToTile(float x, float y, int & ix, int & iy)
    {
        ix = int(std::floor(x*float(RENDER_REGION_SIZE)));
        iy = int(std::floor(y*float(RENDER_REGION_SIZE)));
    }

    bool AbstractWorld::pointOutOfBounds(float x, float y)
    {
        int ix, iy;
        worldToTile(x,y,ix,iy);
        return boundary->outOfBounds(ix,iy);
    }

    bool AbstractWorld::cameraOutOfBounds(float x, float y)
    {
        int ix, iy;
        worldToTile(x,y,ix,iy);
        return boundary->outOfBounds(ix,iy) || boundary->outOfBounds(ix+int(RENDER_REGION_SIZE)-1,iy+int(RENDER_REGION_SIZE)-1);
    }

    TileData AbstractWorld::getTileData(float x, float y)
    {
        Tile h;
        float x0, y0, s;

        worldToTileData(x,y,h,x0,y0,s);

        return TileData(h,x0,y0,s);
    }

}