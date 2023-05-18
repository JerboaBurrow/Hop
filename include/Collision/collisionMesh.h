#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <gl.h>
#include <vector>
#include <cmath>
#include <Maths/vertex.h>
#include <Maths/transform.h>
#include <limits>
#include <cstdint>
#include <memory>
#include <iostream>

using Hop::Maths::Vertex;

namespace Hop::System::Physics
{

    const uint8_t LAST_INSIDE_COUNTER_MAX = 60;

    struct CollisionPrimitive 
    {
        CollisionPrimitive() = default;

        CollisionPrimitive(double x, double y, double r)
        : x(x),y(y),r(r),lastInside(0)
        {}

        virtual ~CollisionPrimitive() = default;
        // x position, y position, radius (model space)
        //  of a collision point
        double x, y, r;
        uint8_t lastInside;

        void setRecentlyInside(int i){ lastInside =  i; }
        bool recentlyInside() const { return lastInside > 0; }
    };

    // struct LineSegment : public CollisionPrimitive 
    // {
    //     LineSegment(double x0, double x1, double y0, double y1, double th = 0.5)
    //     : x0(x0), y0(y0), x1(x1), y1(y1)
    //     {
    //         r = std::sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
    //         double nx = x1 > x0 ? 1.0 : -1.0;
    //         double ny = y1 > y0 ? 1.0 : -1.0;
    //         x = x0 + r/2.0 * nx;
    //         y = y0 + r/2.0 * ny;
    //         thickness = r*th;
    //     }
    //     double x0, x1, y0, y1, thickness;
    // };

    struct Rectangle : public CollisionPrimitive 
    {
        Rectangle() = default;

        Rectangle
        (        
            double llx, double lly,
            double ulx, double uly,
            double urx, double ury,
            double lrx, double lry
        )
        : llx(llx), lly(lly),
          ulx(ulx), uly(uly),
          urx(urx), ury(ury),
          lrx(lrx), lry(lry)
        {
            x = (llx+ulx+urx+lrx)/4.0;
            y = (lly+uly+ury+lry)/4.0;

            double dx = llx-x;
            double dy = lly-y;

            r = std::sqrt(dx*dx+dy*dy);

            lastInside = 0;

            resetAxes();
        }

        void resetAxes()
        {
            axis1x = llx-lrx;
            axis1y = lly-lry;

            x = (llx+ulx+urx+lrx)/4.0;
            y = (lly+uly+ury+lry)/4.0;

            double dx = llx-x;
            double dy = lly-y;

            r = std::sqrt(dx*dx+dy*dy);

            double d = std::sqrt(axis1x*axis1x+axis1y*axis1y);

            axis1x /= d;
            axis1y /= d;

            axis2x = ulx-llx;
            axis2y = uly-lly;

            d = std::sqrt(axis2x*axis2x+axis2y*axis2y);

            axis2x /= d;
            axis2y /= d;
        }

        double height()
        {

            double u = llx-lrx;
            double v = lly-lry;

            return std::sqrt(u*u+v*v);

        }

        double width()
        {

            double u = ulx-llx;
            double v = uly-llx;

            return std::sqrt(u*u+v*v);

        }

        double llx, lly;
        double ulx, uly;
        double urx, ury;
        double lrx, lry;
        double axis1x, axis1y;
        double axis2x, axis2y;
    };

    std::ostream & operator<<(std::ostream & o, Rectangle const & r);

    struct CollisionMesh 
    {
        CollisionMesh()
        : isDebugSetup(false)
        {}
        // construct a mesh around a model space polygon 
        //   with vertices v with each mesh vertex having 
        //   radius r in model space
        //CollisionMesh(std::vector<Vertex> v, double r = 0.01);

        // construct a mesh from given points
        CollisionMesh
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v,
            double x,
            double y, 
            double theta, 
            double scale
        )
        : CollisionMesh(std::move(v))
        {
            updateWorldMesh(x,y,theta,scale);
        }

        CollisionMesh
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v
        )
        : isDebugSetup(false)
        {
            vertices=std::move(v);
            worldVertices.clear();
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                CollisionPrimitive * c = (vertices[i].get());
                Rectangle * l = dynamic_cast<Rectangle*>(c);

                std::shared_ptr<CollisionPrimitive> p;

                if (l != nullptr)
                {
                    p = std::make_shared<Rectangle>
                    (
                        Rectangle
                        (
                            l->llx, l->lly,
                            l->ulx, l->uly,
                            l->urx, l->ury,
                            l->lrx, l->lry
                        )
                    );
                }
                else
                {
                    p = std::make_shared<CollisionPrimitive>
                    (
                        CollisionPrimitive(c->x,c->y,c->r)
                    );
                }

                worldVertices.push_back(std::move(p));
 
            }
        }
        
        size_t size(){return vertices.size();}

        std::shared_ptr<CollisionPrimitive> operator[](size_t i) 
        {
            return worldVertices[i];
        }

        void updateWorldMesh(
            double x,
            double y,
            double theta, 
            double scale
        );

        double momentOfInertia();

        void drawDebug(glm::mat4 & proj);
        void setupDebug();

        ~CollisionMesh()
        {
            glDeleteBuffers(1,&vboQuad);
            glDeleteBuffers(1,&vboP);
            glDeleteBuffers(1,&vboOC);
            glDeleteBuffers(1,&vboOR);
            glDeleteVertexArrays(1,&vaoR);
            glDeleteVertexArrays(1,&vaoC);
            glDeleteBuffers(1,&vboTh);
        }
        
    private:

        std::vector<std::shared_ptr<CollisionPrimitive>> vertices;
        std::vector<std::shared_ptr<CollisionPrimitive>> worldVertices;

        // for debug shapes

        float x, y, theta, scale;

        bool isDebugSetup;

        GLuint vboQuad, vboP, vboOC, vboOR, vboTh, vaoC, vaoR;

        std::vector<float> offsetsCircle, offsetsRectangle;
        std::vector<float> parameters, thickness;

        float quad[6*4] = 
        {
            // positions  / texture coords
            0.5f,  0.5f, 1.0f, 1.0f,   // top right
            0.5f,  -0.5f, 1.0f, 0.0f,   // bottom right
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f, 1.0f,    // top left 
            -0.5f,  -0.5f, 0.0f, 0.0f,   // bottom left
            0.5f,  0.5f, 1.0f, 1.0f  // top right
        };

        GLuint circleShader, rectangleShader;

             const char * collisionPrimitiveVertexShader = "#version " GLSL_VERSION "\n"
    "precision lowp float;\n precision lowp int;\n"
    "layout(location=0) in vec4 a_position;\n"
    "layout(location=1) in vec4 a_offset;\n"
    "uniform mat4 proj;\n"
    "out vec2 texCoord;\n"
    "out vec4 oColour;\n"
    "void main()\n"
    "{\n"
        "gl_Position = proj*vec4(a_offset.w*a_position.xy+a_offset.xy,0.0,1.0);\n"
        "texCoord = a_position.zw;\n"
        "oColour = vec4(0.0,0.0,1.0,1.0);\n"
    "}";

     const char * collisionPrimitiveFragmentShader = "#version " GLSL_VERSION "\n"
    "precision lowp float;\n precision lowp int;\n"
    "in vec4 oColour;\n"
    "in vec2 texCoord;\n"
    "in float theta;\n"
    "out vec4 colour;\n"
    "void main()"
    "{\n"
    "   vec2 c = texCoord-vec2(0.5,0.5);\n"
    "   if (dot(c,c) > 0.5*0.5) {colour = vec4(oColour.rgb,0.1);}\n"
    "   else { colour = oColour; }\n"
    "}";

     const char * rectangleVertexShader = "#version " GLSL_VERSION "\n"
    "precision lowp float;\n precision lowp int;\n"
    "layout(location=0) in vec4 a_position;\n"
    "layout(location=1) in vec4 a_offset;\n"
    "layout(location=2) in vec4 a_parameters;\n"
    "layout(location=3) in float a_thickness;\n"
    "uniform mat4 proj;\n"
    "out vec4 oColour;\n"
    "out vec2 texCoord;\n"
    "out vec4 parameters;\n"
    "out float th;\n"
    "void main()\n"
    "{\n"
        "float cosine = cos(a_offset.z); float sine = sin(a_offset.z);\n"
        "mat2 rot = mat2(cosine,-sine,sine,cosine);"
        "vec2 pos = rot * a_position.xy;\n"
        "gl_Position = proj*vec4(a_offset.w*pos+a_offset.xy,0.0,1.0);\n"
        "texCoord = pos;\n"
        "parameters.xy = rot * a_parameters.xy;\n"
        "parameters.zw = rot * a_parameters.zw;\n"
        "oColour = vec4(0.0,0.0,1.0,1.0);\n"
        "th = a_thickness;\n"
    "}";

     const char * rectangleFragmentShader = "#version " GLSL_VERSION "\n"
    "precision lowp float;\n precision lowp int;\n"
    "in vec4 oColour;\n"
    "in vec2 texCoord;\n"
    "in float th;\n"
    "in vec4 parameters;\n"
    "out vec4 colour;\n"
    "void main()"
    "{\n"
        "vec2 a = parameters.xy; vec2 b = parameters.zw;"
        "float l = length(b-a);"
        "vec2 d = (b-a)/l;\n"
        "vec2 q = texCoord - (a+b)*0.5;\n"
        "q = mat2(d.x,-d.y,d.y,d.x)*q;\n"
        "q = abs(q)-vec2(l*0.5,th);\n"
        "float sd = length(max(q,0.0)) + min(max(q.x,q.y),0.0);"
        "float alpha = 0.1;"
        "if (sd <= 0.0) { colour = vec4(oColour.rgb,0.5); }"
        //"if (sd <= 0.1){ alpha = 1.0-smoothstep(0.0,0.1,sd); colour = vec4(oColour.rgb,alpha); }"
        "else { colour = vec4(oColour.rgb,alpha); }"
    "}";

    };

}

#endif /* COLLISIONMESH_H */
