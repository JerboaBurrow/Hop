#ifndef SPRINGDASHPOTRESOLVER_H
#define SPRINGDASHPOTRESOLVER_H

#include <set>
#include <iterator>
#include <cmath>

#include <Collision/collisionResolver.h>
#include <Collision/collisionMesh.h>
#include <Component/cPhysics.h>

#include <World/tileWorld.h>
#include <World/marchingWorld.h>

#include <Maths/topology.h>
#include <Maths/distance.h>

namespace Hop::System::Physics
{

    using namespace Hop::Object::Component;

    using Hop::World::TileWorld;
    using Hop::World::MarchingWorld;
    using Hop::World::Tile;
    using Hop::World::TileBoundsData;
    using Hop::World::TileNeighbourData;

    const double WALL_MASS_MULTIPLIER = 10.0;
    // will check collision with neigbhours tiles when
    // primitive is less than size*this away
    const double NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER = 1.1;

    class SpringDashpot : public CollisionResolver 
    {
    public:
        SpringDashpot(
            double tc,
            double cor,
            double f
        )
        {
            updateParameters(tc,cor);
            collisionTime = tc;
            coefficientOfRestitution = cor;
            surfaceFriction = f;
        }

        SpringDashpot()
        : collisionTime(1.0/90.0), coefficientOfRestitution(0.75)
        {}

        bool handleObjectObjectCollision(
            Id & objectI, uint64_t particleI,
            Id & objectJ, uint64_t particleJ,
            cCollideable & cI, cCollideable & cJ,
            cPhysics & pI, cPhysics & pJ
        );

        bool handleObjectWorldCollision(
            Id id,
            cCollideable & dataC,
            cPhysics & dataP,
            AbstractWorld * world
        );

        void handleObjectWorldCollisions(
            ComponentArray<cCollideable> & dataC,
            ComponentArray<cPhysics> & dataP,
            std::set<Id>::iterator start,
            std::set<Id>::iterator end,
            AbstractWorld * world
        );

        void springDashpotForceCircles
        (
            cPhysics & pI, cPhysics & pJ,
            double dd, double rx, double ry, double rc, double me,
            double pxi, double pyi, double pxj, double pyj,
            double & fx, double & fy
        );

        void springDashpotWallForceCircle
        (
            double nx,
            double ny,
            double d2,
            double r,
            double m,
            double px, double py,
            cPhysics & dataP,
            double & fx, double & fy
        );

        void springDashpotForceRect
        (
            cPhysics & pI,
            cPhysics & pJ,
            double odod, double nx, double ny,
            double me,
            double px, double py,
            double & fx, double & fy
        );

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            RectanglePrimitive * li,
            RectanglePrimitive * lj,
            bool wall = false
        );

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            CollisionPrimitive * c,
            RectanglePrimitive * l,
            double rx, double ry, double rc, double dd
        );

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            CollisionPrimitive * c,
            CollisionPrimitive * l,
            double rx, double ry, double rc, double dd
        );

        bool handleObjectWorldCollision(
            Id id,
            cCollideable & dataC,
            cPhysics & dataP,
            TileWorld * world
        );

        bool handleObjectWorldCollision(
            Id id,
            cCollideable & dataC,
            cPhysics & dataP,
            MarchingWorld * world
        );
        
        void updateParameters(
            double tc,
            double cor
        );

        void setCoefRestitution(double cor){ return updateParameters(collisionTime, cor); }
        void setSurfaceFriction(double f){ surfaceFriction = f; }

        void tileCollision
        (
            Tile & h,
            double x0,
            double y0,
            std::shared_ptr<CollisionPrimitive> c,
            cPhysics & dataP,
            double & hx,
            double & hy,
            double & lx,
            double & ly,
            double s,
            bool & inside,
            bool & collided,
            bool neighbour = false
        );

        void neighbourTilesCollision
        (
            std::shared_ptr<CollisionPrimitive> c,
            cPhysics & dataP,
            AbstractWorld * world,
            bool & collided
        );

        void tileBoundariesCollision
        (
            std::shared_ptr<CollisionPrimitive> c,
            cPhysics & dataP,
            TileWorld * world,
            bool & collided
        );

        bool tileBoundariesCollisionForce
        (
            std::shared_ptr<CollisionPrimitive> c,
            cPhysics & dataP,
            RectanglePrimitive * li,
            RectanglePrimitive r,
            double lx0, 
            double ly0,
            double lx1, 
            double ly1,
            double nx,
            double ny
        );

        void hardBoundariesCollisionForce
        (
            std::shared_ptr<CollisionPrimitive> c,
            cPhysics & dataP,
            Hop::World::FiniteBoundary bounds,
            float lengthScale,
            bool & collided
        );

    private:

        double collisionTime, coefficientOfRestitution;

        // pre-calculated collision parameters
        double alpha, beta, surfaceFriction;
    };

}
#endif /* SPRINGDASHPOTRESOLVER_H */
