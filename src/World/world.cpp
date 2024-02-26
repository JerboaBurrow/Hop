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
        OrthoCam * c, 
        uint64_t renderRegion, 
        uint64_t dynamicsShell,
        MapSource * f,
        Boundary<double> * b
    )
    : seed(s), camera(c),
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

        mapShader = std::make_unique<jGL::GL::glShader>
        (
            Hop::System::Rendering::marchingQuadVertexShader,
            Hop::System::Rendering::marchingQuadFragmentShader
        );

        mapShader->use();
    }

    /*
    
        Due to the optimisation of the world types
         there is a special projection matrix, rather
         than using the orthoCam's one directly.

         I.e the world is always "at" [0,0]x[1,1] in space
         regardless as to scale, or actual world position.

    */
    void AbstractWorld::updateProjection()
    {
        glm::vec2 resolution = camera->getResolution();
        double maxRes = std::max(resolution.x,resolution.y);
        double zoomLevel = camera->getZoomLevel();
        // scale equally by screen width (all lengths relative to this)
        modelView = glm::scale(glm::mat4(1.0),glm::vec3(maxRes,maxRes,1.0)) *
        // move to position and look at x-y plane from z=1, with up = y axis
        glm::lookAt(
            glm::vec3(0.0,0.0,1.0),
            glm::vec3(0.0,0.0,0.0),
            glm::vec3(0.0,1.0,0.0)
        );

        glm::vec3 center(0.5,0.5, 1.0);
        modelView *= glm::translate(glm::mat4(1.0), center) *
            glm::scale(glm::mat4(1.0),glm::vec3(zoomLevel,zoomLevel,1.0))*
            glm::translate(glm::mat4(1.0), -center);

        // finally, project to the screen (ndc)
        projection = glm::ortho(
        0.0,
        double(resolution.x),
        0.0,
        double(resolution.y)
        );

        vp = projection*modelView;
        invProjection = glm::inverse(vp);
    }

    void AbstractWorld::draw()
    {
        glBindVertexArray(VAO);
        mapShader->use();
        updateProjection();
        
        mapShader->setUniform<glm::mat4>("proj", vp);
        mapShader->setUniform<float>("u_scale", 1.0f);
        mapShader->setUniform<glm::vec4>("u_background", glm::vec4(1.0,1.0,1.0,1.0));
        mapShader->setUniform<glm::vec4>("u_foreground", glm::vec4(221.0f/255.0f,141.0f/255.0f,134.0f/255.0f,1.0));
        mapShader->setUniform<float>("gridWidth", 0.5*gridWidth);

        glDrawArraysInstanced(GL_TRIANGLES,0,6,RENDER_REGION_SIZE*RENDER_REGION_SIZE);

        glBindVertexArray(0);
    }

    void AbstractWorld::worldToTile(float x, float y, int & ix, int & iy)
    {
        ix = int(std::floor(x*float(RENDER_REGION_SIZE)));
        iy = int(std::floor(y*float(RENDER_REGION_SIZE)));
    }

    bool AbstractWorld::pointOutOfBounds(float x, float y)
    {
        return boundary->outOfBounds(x,y);
    }

    bool AbstractWorld::cameraOutOfBounds(float x, float y)
    {
        return boundary->outOfBounds(x,y) || boundary->outOfBounds(x+int(RENDER_REGION_SIZE)-1,y+int(RENDER_REGION_SIZE)-1);
    }

    TileData AbstractWorld::getTileData(float x, float y)
    {
        Tile h;
        double x0, y0, s;
        int i,j;

        worldToTileData(x,y,h,x0,y0,s,i,j);

        return TileData(h,x0,y0,s);
    }

}