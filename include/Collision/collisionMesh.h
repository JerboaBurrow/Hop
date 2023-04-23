#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <vector>
#include <cmath>
#include <Maths/vertex.h>
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

    struct LineSegment : public CollisionPrimitive 
    {
        LineSegment(double x0, double x1, double y0, double y1, double th = 0.1)
        : x0(x0), y0(y0), x1(x1), y1(y1)
        {
            r = std::sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
            double nx = x1 > x0 ? 1.0 : -1.0;
            double ny = y1 > y0 ? 1.0 : -1.0;
            x = x0 + r/2.0 * nx;
            y = y0 + r/2.0 * ny;
            thickness = r*th;
        }
        double x0, x1, y0, y1, thickness;
    };

    struct CollisionMesh 
    {
        CollisionMesh(){}
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
                LineSegment * l = dynamic_cast<LineSegment*>(c);

                std::shared_ptr<CollisionPrimitive> p;

                if (l != nullptr)
                {
                    p = std::make_shared<CollisionPrimitive>
                    (
                        LineSegment(l->x0, l->x1, l->y0, l->y1)
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

    private:
        std::vector<std::shared_ptr<CollisionPrimitive>> vertices;
        std::vector<std::shared_ptr<CollisionPrimitive>> worldVertices;
    };

}

#endif /* COLLISIONMESH_H */
