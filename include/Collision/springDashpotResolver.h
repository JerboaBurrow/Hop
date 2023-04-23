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
    const double NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER = 1.5;

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
            friction = f;
        }

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

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            LineSegment * li,
            LineSegment * lj,
            double rx, double ry, double rc, double dd
        );

        void collisionForce
        (
            cPhysics & pI, cPhysics & pJ,
            CollisionPrimitive * c,
            LineSegment * l,
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

        void applyForce
        (
            double nx,
            double ny,
            double d2,
            double c,
            cPhysics & dataP
        );

    private:

        double collisionTime, coefficientOfRestitution;

        // pre-calculated collision parameters
        double alpha, beta, friction;
        double kr, kd;
    };

}
#endif /* SPRINGDASHPOTRESOLVER */
