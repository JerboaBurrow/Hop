#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <vector>
#include <cmath>
#include <Object/vertex.h>

struct CollisionVertex {
    CollisionVertex(double x, double y, double r)
    : x(x),y(y),r(r)
    {}
    // x position, y position, radius (model space)
    //  of a collision point
    double x;
    double y;
    double r;
};

struct CollisionMesh {
    CollisionMesh(){}
    // construct a mesh around a model space polygon 
    //   with vertices v with each mesh vertex having 
    //   radius r in model space
    CollisionMesh(std::vector<Vertex> v, double r = 0.01);
    // construct a mesh from given points
    CollisionMesh(std::vector<CollisionVertex> v){vertices=v;}
    std::vector<CollisionVertex> vertices;
};

#endif /* COLLISIONMESH_H */