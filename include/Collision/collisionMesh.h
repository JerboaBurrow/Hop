#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

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
        Rectangle()
        : Rectangle(0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0)
        {}

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
            double v = uly-lly;

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
            computeRadius();
        }
        
        size_t size(){return vertices.size();}

        std::shared_ptr<CollisionPrimitive> getModelVertex(size_t i)
        {
            return vertices[i];
        }

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
        void computeRadius();
        double getRadius(){return radius;}

        double getX(){return x;}
        double getY(){return y;}
        double getTheta(){return theta;}
        double getScale(){return scale;}
        
    private:

        std::vector<std::shared_ptr<CollisionPrimitive>> vertices;
        std::vector<std::shared_ptr<CollisionPrimitive>> worldVertices;

        double radius, x, y, theta, scale;

    };

}

#endif /* COLLISIONMESH_H */
