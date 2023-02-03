#ifndef CELLLIST_H
#define CELLLIST_H

#include <Collision/collisionDetector.h>
#include <memory>
#include <utility>

const uint64_t MAX_PARTICLES = 100000;
const uint64_t MAX_PARTICLES_PER_CELL = 64;
const uint64_t NULL_INDEX = MAX_PARTICLES+1;

class CellList : public CollisionDetector {
public:
    CellList(uint64_t n,tupled lx = tupled(0.0,1.0), tupled ly = tupled(0.0,1.0))
    : CollisionDetector(lx,ly),rootNCells(n), nCells(n*n)
    {
        lX = limX.second-limX.first;
        lY = limY.second-limY.first;

        dx = lX / double(rootNCells);
        dy = lY / double(rootNCells);

        cells = std::make_unique<uint64_t[]>(nCells*MAX_PARTICLES_PER_CELL);
        lastElementInCell = std::make_unique<uint64_t[]>(nCells);
        id = std::make_unique<std::pair<std::string,uint64_t>[]>(MAX_PARTICLES);

        lastElement = NULL_INDEX;
        for (int i = 0; i < nCells; i++){
            for (int j = 0; j < MAX_PARTICLES_PER_CELL; j++){
                cells[i*MAX_PARTICLES_PER_CELL+j] = NULL_INDEX;
            }
            lastElementInCell[i] = NULL_INDEX;
        }
        for (int i = 0; i < MAX_PARTICLES; i++){
            id[i] = std::pair("",NULL_INDEX);
        }
    }
private:

    uint64_t rootNCells, nCells; // rootNCells x rootNCells grid

    double dx, dy;

    std::unique_ptr<uint64_t[]> cells;
    std::unique_ptr<uint64_t[]> lastElementInCell;
    std::unique_ptr<std::pair<std::string,uint64_t>[]> id;
    uint64_t lastElement;

    uint64_t hash(double x, double y){
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

    void handleObjectCollisions(
        ObjectManager * manager,
        CollisionResolver * resolver,
        std::set<Id> objects
    );
    
    void handleWorldCollisions(
        ObjectManager * manager,
        CollisionResolver * resolver,
        World * world,
        std::set<Id> objects
    );

};

#endif /* CELLLIST_H */
