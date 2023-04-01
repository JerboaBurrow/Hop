#ifndef SPRINGDASHPOTRESOLVER
#define SPRINGDASHPOTRESOLVER

#include <Collision/collisionResolver.h>
#include <cmath>
#include <Object/objectManager.h>
#include <Collision/collisionMesh.h>

#include <World/tileWorld.h>
#include <World/marchingWorld.h>

#include <Maths/topology.h>
#include <Maths/distance.h>

namespace Hop::System::Physics
{

    using namespace Hop::Object::Component;
    using Hop::World::AbstractWorld;

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
            ObjectManager * manager
        );

        void handleObjectWorldCollisions(
            Id id,
            ObjectManager * manager,
            AbstractWorld * world
        );

        void handleObjectWorldCollisions(
            Id id,
            ObjectManager * manager,
            TileWorld * world
        );

        void handleObjectWorldCollisions(
            Id id,
            ObjectManager * manager,
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
            CollisionVertex & c,
            cPhysics & dataP,
            double & hx,
            double & hy,
            double & lx,
            double & ly,
            bool & inside
        );

        void neighbourTilesCollision
        (
            CollisionVertex & c,
            cPhysics & dataP,
            TileNeighbourData & tileNieghbours
        );

        void tileBoundariesCollision
        (
            CollisionVertex & c,
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
