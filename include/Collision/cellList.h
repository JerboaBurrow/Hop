#ifndef CELLLIST_H
#define CELLLIST_H

#include <Collision/collisionDetector.h>
#include <memory>
#include <utility>

namespace Hop::System::Physics
{
    const uint64_t MAX_PARTICLES = 100000;
    const uint64_t MAX_PARTICLES_PER_CELL = 64;
    const uint64_t NULL_INDEX = MAX_PARTICLES+1;

    class CellList : public CollisionDetector 
    {
    public:

        CellList(uint64_t n, tupled lx = tupled(0.0,1.0), tupled ly = tupled(0.0,1.0));

    private:

        uint64_t rootNCells, nCells; // rootNCells x rootNCells grid

        double dx, dy;

        std::unique_ptr<uint64_t[]> cells;
        std::unique_ptr<uint64_t[]> lastElementInCell;
        std::unique_ptr<std::pair<std::string,uint64_t>[]> id;
        uint64_t lastElement;

        uint64_t hash(double x, double y)
        {
            return std::floor((x-limX.first)/dx)*rootNCells+std::floor((y-limY.first)/dy);
        }

        void clear(bool full = false);

        void populate(ObjectManager * manager, std::set<Id> objects);

        void cellCollisions(
            uint64_t a1,
            uint64_t b1,
            uint64_t a2,
            uint64_t b2,
            ObjectManager * manager,
            CollisionResolver * resolver
        );

        void handleObjectObjectCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            std::set<Id> objects
        );

        void cellCollisionsThreaded(
            ObjectManager * manager,
            CollisionResolver * resolver,
            int a
        );
        
        void handleObjectWorldCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            World * world,
            std::set<Id> objects
        );


        void handleObjectWorldCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            TileWorld * world,
            std::set<Id> objects
        );


        void handleObjectWorldCollisions(
            ObjectManager * manager,
            CollisionResolver * resolver,
            MarchingWorld * world,
            std::set<Id> objects
        );

    };

}

#endif /* CELLLIST_H */
