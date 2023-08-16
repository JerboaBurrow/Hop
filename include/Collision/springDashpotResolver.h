#ifndef SPRINGDASHPOTRESOLVER
#define SPRINGDASHPOTRESOLVER

#include <Collision/collisionResolver.h>
#include <cmath>
#include <Collision/collisionMesh.h>

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
    const double NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER = 2.0;

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
            friction = f;
        }

        SpringDashpot()
        : collisionTime(1.0/90.0), coefficientOfRestitution(0.75)
        {}

        void handleObjectObjectCollision(
            Id & objectI, uint64_t particleI,
            Id & objectJ, uint64_t particleJ,
            cCollideable & cI, cCollideable & cJ,
            cPhysics & pI, cPhysics & pJ
        );

        void handleObjectWorldCollision(
            Id id,
            cCollideable & dataC,
            cPhysics & dataP,
            AbstractWorld * world
        );

        void springDashpotForce
        (
            cPhysics & pI, cPhysics & pJ,
            double dd, double rx, double ry, double rc,
            double pxi, double pyi, double pxj, double pyj
        );

        void springDashpotWallForce
        (
            double nx,
            double ny,
            double d2,
            double c,
            double px, double py,
            cPhysics & dataP
        );

        void springDashpotForce
        (
            cPhysics & pI,
            cPhysics & pJ,
            double odod, double nx, double ny,
            double px, double py
        );

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            Rectangle * li,
            Rectangle * lj,
            bool wall = false
        );

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            CollisionPrimitive * c,
            Rectangle * l,
            double rx, double ry, double rc, double dd
        );

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            CollisionPrimitive * c,
            CollisionPrimitive * l,
            double rx, double ry, double rc, double dd
        );

        void handleObjectWorldCollision(
            Id id,
            cCollideable & dataC,
            cPhysics & dataP,
            TileWorld * world
        );

        void handleObjectWorldCollision(
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
            bool neighbour = false
        );

        void neighbourTilesCollision
        (
            std::shared_ptr<CollisionPrimitive> c,
            cPhysics & dataP,
            TileNeighbourData & tileNieghbours
        );

        void tileBoundariesCollision
        (
            std::shared_ptr<CollisionPrimitive> c,
            cPhysics & dataP,
            TileBoundsData & tileBounds
        );

    private:

        double collisionTime, coefficientOfRestitution;

        // pre-calculated collision parameters
        double alpha, beta, friction;
        double kr, kd, krR, kdR, alphaR, betaR;
    };

}
#endif /* SPRINGDASHPOTRESOLVER */
