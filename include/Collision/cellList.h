#ifndef CELLLIST_H
#define CELLLIST_H

#include <Collision/collisionDetector.h>
#include <Collision/springDashpotResolver.h>
#include <memory>
#include <utility>

#include <World/tileWorld.h>
#include <World/marchingWorld.h>

#include <Component/componentArray.h>

#include <chrono>
using namespace std::chrono;

namespace Hop::System::Physics
{

    using Hop::World::TileWorld;
    using Hop::World::MarchingWorld;

    const uint64_t MAX_PARTICLES = 100000;
    const uint64_t MAX_PARTICLES_PER_CELL = 64;
    const uint64_t NULL_INDEX = MAX_PARTICLES+1;

    class CellList : public CollisionDetector 
    {
    public:

        CellList(AbstractWorld * world);
        
        CellList(uint64_t n, tupled lx = tupled(0.0,1.0), tupled ly = tupled(0.0,1.0));

        uint64_t getRootNCells(){ return rootNCells; }

    private:

        uint64_t rootNCells, nCells; // rootNCells x rootNCells grid

        double dx, dy;

        std::unique_ptr<uint64_t[]> cells;
        std::unique_ptr<uint64_t[]> lastElementInCell;
        
        std::unique_ptr<bool[]> notEmpty;
        uint64_t countNotEmpty;

        std::unique_ptr<std::pair<Id,uint64_t>[]> id;
        uint64_t lastElement;

        uint64_t hash(double x, double y)
        {
            return std::floor((x-limX.first)/dx)*rootNCells+std::floor((y-limY.first)/dy);
        }

        void clear(bool full = false);

        void populate
        (      
            ComponentArray<cCollideable> & dataC,
            ComponentArray<cPhysics> & dataP,
            std::set<Id> objects
        );

        void cellCollisions(
            uint64_t a1,
            uint64_t b1,
            uint64_t a2,
            uint64_t b2,
            ComponentArray<cCollideable> & dataC,
            ComponentArray<cPhysics> & dataP,
            CollisionResolver * resolver
        );

        void cellCollisionsThreaded(
            uint64_t a1,
            uint64_t b1,
            uint64_t a2,
            uint64_t b2,
            cCollideable * dataC,
            dense_hash_map<Id,uint64_t> & idToIndexC,
            cPhysics * dataP,
            dense_hash_map<Id,uint64_t> & idToIndexP,
            CollisionResolver * resolver
        );

        void handleObjectObjectCollisionsThreaded(
            cCollideable * dataC,
            dense_hash_map<Id,uint64_t> & idToIndexC,
            cPhysics * dataP,
            dense_hash_map<Id,uint64_t> & idToIndexP,
            CollisionResolver * resolver,
            std::pair<unsigned,unsigned> * jobs,
            unsigned njobs
        );

        void handleObjectObjectCollisions(
            ComponentArray<cCollideable> & dataC,
            ComponentArray<cPhysics> & dataP,
            CollisionResolver * resolver,
            std::set<Id> objects,
            ThreadPool * workers = nullptr
        );

        void handleObjectWorldCollisions(
            ComponentArray<cCollideable> & dataC,
            ComponentArray<cPhysics> & dataP,
            CollisionResolver * resolver,
            AbstractWorld * world,
            std::set<Id> objects,
            ThreadPool * workers = nullptr
        );

    };

}

#endif /* CELLLIST_H */
