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
            double scale
        )
        : mesh(CollisionMesh(v,x,y,theta,scale)), x(x), y(y)
        {}

        cCollideable(){}

        void updateWorldMesh(
            double x,
            double y,
            double theta, 
            double scale
        )
        {
            mesh.updateWorldMesh(x, y, theta, scale);
            this->x = x;
            this->y = y;
        }

    };

}

#endif /* CCOLLIDEABLE_H */
