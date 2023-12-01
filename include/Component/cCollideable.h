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

        double x, y;

        cCollideable
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v,
            double x,
            double y, 
            double theta, 
            double scale,
            double stiffness = CollisionPrimitive::RIGID,
            double damping = 1.0,
            double mass = 1.0
        )
        : mesh(CollisionMesh(v,x,y,theta,scale, stiffness, damping, mass)), x(x), y(y)
        {}

        cCollideable(){}

        void updateMesh(
            double x,
            double y,
            double theta, 
            double scale,
            double dt
        )
        {
            mesh.updateWorldMesh(x, y, theta, scale, dt);
            this->x = mesh.getX();
            this->y = mesh.getY();
        }

    };

}

#endif /* CCOLLIDEABLE_H */
