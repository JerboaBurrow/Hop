#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <vector>

struct CollisionVertex {
    // x position, y position, radius (model space)
    //  of a collision point
    double x;
    double y;
    double r;
};

struct Vertex {
    float x;
    float y;
};

struct CollisionMesh {

    // construct a mesh around a model space polygon 
    //   with vertices v with each mesh vertex having 
    //   radius r in model space
    CollisionMesh(std::vector<Vertex> v, double r = 0.01);
    // construct a mesh from given points
    CollisionMesh(std::vector<CollisionVertex> v){vertices=v;}
    std::vector<CollisionVertex> vertices;
};

// TODO port from kotlin

// Decorate a polygon defined by edges with circles of radius r
//  by placing circles inside the polygon that just touch the
//  perimeter
//      - tight corners will be poorly handled
//      - some circles will overlap (redundancy)
//      + quick
//      + generative
//      + all circles touch edges from inside (do not overextend past edges)
//      + smaller radius alleviates the -'s
// fun circleMesh(edges: List<Vertex>, r: Double = 0.1): CircleMesh {
//     val e = edges.toMutableList()
//     val mesh = mutableListOf<Circle>()

//     for (i in 0 until e.size){
//         // walk through perimeter
//         val n = e[(i+1)%e.size]-e[i]
//         // inward normal
//         var nt = Vertex(n.y,-n.x)
//         nt /= norm(nt)
//         val d = norm(n)
//         if (d > 0){
//             val dm = floor(d/(2.0*r)).toInt()
//             var m = 0.0
//             // walk across this edge
//             for (j in 0 until dm+1){
//                 val c = e[i]+n/d * m
//                 if (j in 1 until dm){
//                     // move inside, not a corner point
//                     c += nt*r
//                 }
//                 else if (m == 0.0){
//                     // move circle so that it is tangent to both
//                     // edges simultaneously
//                     val va = if(i-1 < 0) {e.size-1} else {i-1}
//                     val vb = i
//                     val vc = (i+1)%e.size
//                     // bisector direction
//                     val ray = angleBisectorRay(e[va],e[vb],e[vc])
//                     // angle at vertex i
//                     val theta = angle(e[va],e[vb],e[vc])
//                     // hypotenuse length is distance to move, circle radius is opposite side length
//                     c += ray/norm(ray)*(r/sin(theta/2.0))
//                 }
//                 else if (j == dm){
//                     // move circle so that it is tangent to both
//                     // edges simultaneously
//                     val va = i
//                     val vb = (i+1)%e.size
//                     val vc = (i+2)%e.size
//                     // bisector direction
//                     val ray = angleBisectorRay(e[va],e[vb],e[vc])
//                     // angle at vertex i+1 % n
//                     val theta = angle(e[va],e[vb],e[vc])
//                     // hypotenuse length is distance to move, circle radius is opposite side length
//                     c += ray/norm(ray)*(r/sin(theta/2.0))
//                 }
//                 mesh.add(Circle(c.x,c.y,r))
//                 m += 2.0*r
//             }
//         }
//     }
//     return CircleMesh(mesh.toList())
// }

#endif /* COLLISIONMESH_H */
