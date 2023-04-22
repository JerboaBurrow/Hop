#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <vector>
#include <cmath>
#include <Maths/vertex.h>
#include <limits>
#include <cstdint>

using Hop::Maths::Vertex;

namespace Hop::System::Physics
{

    const uint8_t LAST_INSIDE_COUNTER_MAX = 60;

    struct CollisionVertex 
    {
        CollisionVertex(double x, double y, double r)
        : x(x),y(y),r(r),lastInside(0)
        {}
        // x position, y position, radius (model space)
        //  of a collision point
        double x;
        double y;
        double r;
        uint8_t lastInside;

        void setRecentlyInside(int i){ lastInside =  i; }
        bool recentlyInside() const { return lastInside > 0; }
    };

    bool operator==(const CollisionVertex & lhs, const CollisionVertex & rhs);

    struct CollisionMesh 
    {
        CollisionMesh(){}
        // construct a mesh around a model space polygon 
        //   with vertices v with each mesh vertex having 
        //   radius r in model space
        CollisionMesh(std::vector<Vertex> v, double r = 0.01);
        // construct a mesh from given points
        CollisionMesh
        (
            std::vector<CollisionVertex> v,
            double x,
            double y, 
            double theta, 
            double scale
        )
        {
            vertices=v;
            worldVertices=v;
            updateWorldMesh(x,y,theta,scale);
        }

        CollisionMesh
        (
            std::vector<CollisionVertex> v
        )
        {
            vertices=v;
            worldVertices=v;
        }

        size_t size(){return vertices.size();}
        CollisionVertex & operator[](size_t i) 
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
        std::vector<CollisionVertex> vertices;
        std::vector<CollisionVertex> worldVertices;
    };

}

#endif /* COLLISIONMESH_H */
