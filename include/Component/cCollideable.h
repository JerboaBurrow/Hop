#ifndef CCOLLIDEABLE_H
#define CCOLLIDEABLE_H

#include <Collision/collisionMesh.h>

namespace Hop::Object::Component
{
    using Hop::System::Physics::CollisionMesh;
    using Hop::System::Physics::CollisionPrimitive;

    struct cCollideable 
    {
        CollisionMesh mesh;

        cCollideable
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v,
            double x,
            double y, 
            double theta, 
            double scale
        )
        : mesh(CollisionMesh(v,x,y,theta,scale))
        {}

        cCollideable(CollisionMesh & m)
        : mesh(m)
        {}

        cCollideable(){}

        void updateMesh(
            cTransform & transform,
            cPhysics & physics,
            double dt
        )
        {
            mesh.updateWorldMesh(transform, physics, dt);
        }

        void add
        (
            std::shared_ptr<CollisionPrimitive> c, 
            cTransform & transform
        ) 
        { 
            mesh.add(c); 
            cPhysics phys(transform.x, transform.y, transform.theta);
            mesh.updateWorldMesh(transform, phys, 0.0);
        }

        void remove(size_t i) { mesh.remove(i); }

    };

}

#endif /* CCOLLIDEABLE_H */
