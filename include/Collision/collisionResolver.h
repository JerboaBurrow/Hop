#ifndef COLLISIONRESOLVER_H
#define COLLISIONRESOLVER_H

#include <string>
#include <Object/id.h>

#include <jThread/jThread.h>

#include <World/world.h>

#include <Component/componentArray.h>
#include <Component/cPhysics.h>
#include <Component/cCollideable.h>

namespace Hop::World 
{
    class AbstractWorld;
}

namespace Hop::System::Physics
{
        
    using Hop::Object::Id;

    using Hop::Object::Component::ComponentArray;
    using Hop::Object::Component::cCollideable;
    using Hop::Object::Component::cPhysics;
    using Hop::World::AbstractWorld;

    class CollisionResolver 
    {

    public:

        CollisionResolver(){}

        virtual ~CollisionResolver() = default;
        
        virtual void handleObjectObjectCollision
        (
            Id & objectI, uint64_t particleI,
            Id & objectJ, uint64_t particleJ,
            cCollideable & cI, cCollideable & cJ,
            cPhysics & pI, cPhysics & pJ
        ) = 0;

        virtual void handleObjectWorldCollision
        (
            Id id,
            cCollideable & dataC,
            cPhysics & dataP,
            AbstractWorld * world
        ) = 0;

        virtual void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            RectanglePrimitive * li,
            RectanglePrimitive * lj,
            bool wall
        ) = 0;

        virtual void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            CollisionPrimitive * c,
            RectanglePrimitive * l,
            double rx, double ry, double rc, double dd
        ) = 0;

        virtual void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            CollisionPrimitive * c,
            CollisionPrimitive * l,
            double rx, double ry, double rc, double dd
        ) = 0;

        virtual void setCoefRestitution(double cor) = 0;
        virtual void setSurfaceFriction(double f) = 0;

    };

}

#endif /* COLLISIONRESOLVER_H */
