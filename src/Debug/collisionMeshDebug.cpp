#include <Debug/collisionMeshDebug.h>

namespace Hop::Debugging
{

    void CollisionMeshDebug::setupGL(bool reset)
    {
        if (reset)
        {
            freeGL();
            circles = std::vector<float>(cachedCircles*4,0.0f);
            rectanglesOffset = std::vector<float>(cachedRects*4,0.0f);
            rectanglesParameters = std::vector<float>(cachedRects*4,0.0f);
            rectanglesThickness = std::vector<float>(cachedRects,0.0f);
            
            uploadedCircles = cachedCircles;
            uploadedRects = cachedRects;

            glGenBuffers(1, &quadVBO);
            glGenBuffers(1, &cOffset);
            glGenBuffers(1, &rThickness);
            glGenBuffers(1, &rOffset);
            glGenBuffers(1, &rParameters);
            glGenVertexArrays(1, &cVao);
            glGenVertexArrays(1, &rVao);
        }

        glBindVertexArray(cVao);
        
        glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*6*sizeof(float),
            &quad[0],
            GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(0);
            glVertexAttribPointer(
                0,
                4,
                GL_FLOAT,
                false,
                4*sizeof(float),
                0
        );
        glVertexAttribDivisor(0,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        glBindBuffer(GL_ARRAY_BUFFER, cOffset);
        glBufferData
        (
            GL_ARRAY_BUFFER,
            4*sizeof(float)*circles.size(),
            &circles[0],
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer
        (
            1,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(1,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        // RECTANGLES

        glBindVertexArray(rVao);

        glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*6*sizeof(float),
            &quad[0],
            GL_STATIC_DRAW
        );
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(0,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        // offsets
        glBindBuffer(GL_ARRAY_BUFFER,rOffset);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*sizeof(float)*cachedRects,
            &rectanglesOffset[0],
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(1,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        // parameters
        glBindBuffer(GL_ARRAY_BUFFER,rParameters);
        glBufferData(
            GL_ARRAY_BUFFER,
            4*sizeof(float)*cachedRects,
            &rectanglesParameters[0],
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            4,
            GL_FLOAT,
            false,
            4*sizeof(float),
            0
        );
        glVertexAttribDivisor(2,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        // thickness
        glBindBuffer(GL_ARRAY_BUFFER,rThickness);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(float)*cachedRects,
            &rectanglesThickness[0],
            GL_DYNAMIC_DRAW
        );
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3,
            1,
            GL_FLOAT,
            false,
            sizeof(float),
            0
        );
        glVertexAttribDivisor(3,1);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        glBindVertexArray(0);

        circleShader = glCreateProgram();
        Hop::GL::compileShader(circleShader,collisionPrimitiveVertexShader,collisionPrimitiveFragmentShader);
        
        rectangleShader = glCreateProgram();
        Hop::GL::compileShader(rectangleShader,rectangleVertexShader,rectangleFragmentShader);

        uploadedCircles = cachedCircles;
        uploadedRects = cachedRects;

    }

    void CollisionMeshDebug::debugCollisionMesh(EntityComponentSystem * m, glm::mat4 & proj)
    {

        auto objects = m->getObjects();  

        auto citer = objects.cbegin();
        auto cend = objects.cend();

        double x, y, theta, scale;

        nCircles = 0;
        nRectangles = 0;

        while (citer != cend)
        {
          if (m->hasComponent<cCollideable>(citer->first))
          {
            
            cCollideable & c = m->getComponent<cCollideable>(citer->first);

            x = c.mesh.getX();
            y = c.mesh.getY();
            theta = c.mesh.getTheta();
            scale = c.mesh.getScale();

            for (unsigned i = 0; i < c.mesh.size(); i++)
            {
                CollisionPrimitive * cp = (c.mesh[i].get());
                CollisionPrimitive * cpmodel = c.mesh.getModelVertex(i).get();
                Rectangle * r = dynamic_cast<Rectangle*>(cp);

                if (r != nullptr)
                {
                    if (nRectangles > cachedRects)
                    {
                        rectanglesOffset.insert(rectanglesOffset.end(), 64*4, 0.0f);
                        rectanglesParameters.insert(rectanglesParameters.end(), 64*4, 0.0f);
                        rectanglesThickness.insert(rectanglesThickness.end(), 64, 0.0f);
                        cachedRects += 64;
                    }

                    // thickness
                    float thx = r->llx-r->lrx;
                    float thy = r->lly-r->lry;
                    float th = std::sqrt(thx*thx+thy*thy);
                    // line through middle
                    float ax = r->llx - r->axis1x*th*0.5;
                    float ay = r->lly - r->axis1y*th*0.5;
                    float bx = r->ulx - r->axis1x*th*0.5;
                    float by = r->uly - r->axis1y*th*0.5;

                    rectanglesParameters[nRectangles*4] = ax;
                    rectanglesParameters[nRectangles*4+1] = ay;
                    rectanglesParameters[nRectangles*4+2] = bx;
                    rectanglesParameters[nRectangles*4+3] = by;

                    rectanglesOffset[nRectangles*4] = x;
                    rectanglesOffset[nRectangles*4+1] = y;
                    rectanglesOffset[nRectangles*4+2] = theta;
                    rectanglesOffset[nRectangles*4+3] = scale*2.0;

                    rectanglesThickness[nRectangles] = th*0.5;

                    nRectangles += 1;
                }
                else
                {
                    if (nCircles > cachedCircles)
                    {
                        circles.insert(circles.end(), 64*4, 0.0f);
                        cachedCircles += 64;
                    }

                    circles[nCircles*4] = cp->x;
                    circles[nCircles*4+1] = cp->y;
                    circles[nCircles*4+2] = theta;
                    circles[nCircles*4+3] = scale*2.0*cpmodel->r;

                    nCircles += 1;

                }

            }

          }
          citer++;
        }

        if (cachedCircles > uploadedCircles || cachedRects > uploadedRects)
        {
            setupGL(true);
        }

        if (nCircles > 0)
        {

            glBindVertexArray(cVao);
        
            glBindBuffer(GL_ARRAY_BUFFER, cOffset);

            glBufferSubData
            (
                GL_ARRAY_BUFFER,
                0,
                4*sizeof(float)*nCircles,
                &circles[0]
            );

            glBindBuffer(GL_ARRAY_BUFFER,0);
            
            glBindVertexArray(0);

            glBindVertexArray(cVao);

            glUseProgram(circleShader);

            glUniformMatrix4fv(
                glGetUniformLocation(circleShader,"proj"),
                1, 0, &proj[0][0]
            );

            glDrawArraysInstanced(GL_TRIANGLES,0,6,nCircles);

            glBindVertexArray(0);
        }

        if (nRectangles > 0)
        {

            glBindVertexArray(rVao);

            glBindBuffer(GL_ARRAY_BUFFER, rOffset);
            glBufferSubData
            (
                GL_ARRAY_BUFFER,
                0,
                4*sizeof(float)*nRectangles,
                &rectanglesOffset[0]
            );
            glBindBuffer(GL_ARRAY_BUFFER,0);

            glBindBuffer(GL_ARRAY_BUFFER, rParameters);
            glBufferSubData
            (
                GL_ARRAY_BUFFER,
                0,
                4*sizeof(float)*nRectangles,
                &rectanglesParameters[0]
            );

            glBindBuffer(GL_ARRAY_BUFFER,0);

            glBindBuffer(GL_ARRAY_BUFFER, rThickness);
            glBufferSubData
            (
                GL_ARRAY_BUFFER,
                0,
                sizeof(float)*nRectangles,
                &rectanglesThickness[0]
            );
            glBindBuffer(GL_ARRAY_BUFFER,0);
            
            glUseProgram(rectangleShader);

            glUniformMatrix4fv(
                glGetUniformLocation(rectangleShader,"proj"),
                1, 0, &proj[0][0]
            );

            glDrawArraysInstanced(GL_TRIANGLES,0,6,nRectangles);

            glBindVertexArray(0);
        }


    }

}