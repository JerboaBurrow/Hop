#include <Collision/cellList.h>
#include <iostream>
#include <chrono>
using namespace std::chrono;

namespace Hop::System::Physics
{

    using Hop::Object::NULL_ID;
    
    CellList::CellList(AbstractWorld * world)
    {
        tupled dynamicsCoords = world->getWorldWidth();
        double width = (dynamicsCoords.second-dynamicsCoords.first)/2.0;
        
        double unit = 0.1*world->worldUnitLength();
        unsigned n = std::ceil(width/(2.0*unit));

        limX = dynamicsCoords;
        limY = dynamicsCoords;
        
        rootNCells = n;
        nCells = n*n;

        lX = limX.second-limX.first;
        lY = limY.second-limY.first;

        dx = lX / double(rootNCells);
        dy = lY / double(rootNCells);

        cells = std::make_unique<uint64_t[]>(nCells*MAX_PARTICLES_PER_CELL);
        lastElementInCell = std::make_unique<uint64_t[]>(nCells);
        id = std::make_unique<std::pair<Id,uint64_t>[]>(MAX_PARTICLES);
        notEmpty = std::make_unique<bool[]>(nCells);

        lastElement = NULL_INDEX;
        for (int i = 0; i < nCells; i++)
        {
            notEmpty[i] = false;
            for (int j = 0; j < MAX_PARTICLES_PER_CELL; j++)
            {
                cells[i*MAX_PARTICLES_PER_CELL+j] = NULL_INDEX;
            }
            
            lastElementInCell[i] = NULL_INDEX;
        }
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            id[i] = std::pair(NULL_ID,NULL_INDEX);
        }

    }

    CellList::CellList(
        uint64_t n,
        tupled lx, 
        tupled ly
    )
    : CollisionDetector(lx,ly),rootNCells(n), nCells(n*n)
    {
        lX = limX.second-limX.first;
        lY = limY.second-limY.first;

        dx = lX / double(rootNCells);
        dy = lY / double(rootNCells);

        cells = std::make_unique<uint64_t[]>(nCells*MAX_PARTICLES_PER_CELL);
        lastElementInCell = std::make_unique<uint64_t[]>(nCells);
        id = std::make_unique<std::pair<Id,uint64_t>[]>(MAX_PARTICLES);
        notEmpty = std::make_unique<bool[]>(nCells);

        lastElement = NULL_INDEX;
        for (int i = 0; i < nCells; i++)
        {
            notEmpty[i] = false;
            for (int j = 0; j < MAX_PARTICLES_PER_CELL; j++)
            {
                cells[i*MAX_PARTICLES_PER_CELL+j] = NULL_INDEX;
            }
            
            lastElementInCell[i] = NULL_INDEX;
        }
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            id[i] = std::pair(NULL_ID,NULL_INDEX);
        }
    }

    void CellList::clear(bool full)
    {
        for (int l = 0; l < nCells; l++)
        {
            uint64_t last;
            notEmpty[l] = false;
            
            if (full){last = MAX_PARTICLES_PER_CELL;}
            
            if (!full){if (lastElementInCell[l]!= NULL_INDEX){last = lastElementInCell[l]+1;}else{last=0;}}
            
            if (last == NULL_INDEX){last = 0;}

            if (last >= 0)
            {
                for (int i = 0; i < last; i++)
                {
                    cells[l*MAX_PARTICLES_PER_CELL+i] = NULL_INDEX;
                }
            }

            lastElementInCell[l] = NULL_INDEX;
        }

        uint64_t last;

        if (full){last = MAX_PARTICLES;}

        if (!full){if (lastElement != NULL_INDEX){last = lastElement+1;}else{last=0;}}
        
        if (last == NULL_INDEX){last = 0;}
        
        if (last >= 0)
        {
            for (int i = 0; i < last; i++)
            {
                id[i] = std::pair(NULL_ID,NULL_INDEX);
            }
        }

        lastElement = NULL_INDEX;
    }

    void CellList::populate(
        ComponentArray<cCollideable> & dataC,
        ComponentArray<cPhysics> & dataP,
        std::set<Id> objects
    )
    {
        clear();

        int o = 0;
        for (auto it = objects.begin(); it != objects.cend(); it++)
        {

            cCollideable & data = dataC.get(*it);

            uint64_t meshSize = data.mesh.size();

            for (int i = 0; i < meshSize; i++)
            {
                const CollisionVertex p = data.mesh[i];

                uint64_t h = hash(p.x,p.y);

                if (0 <= h && h < nCells)
                {
                    if (lastElementInCell[h] == NULL_INDEX || lastElementInCell[h] < MAX_PARTICLES_PER_CELL)
                    {
                        uint64_t next, nextId;

                        if (lastElementInCell[h]==NULL_INDEX){next = 0;}
                        else {next = lastElementInCell[h]+1;}

                        if (lastElement == NULL_INDEX){nextId = 0;}
                        else {nextId = lastElement+1;} 

                        cells[h*MAX_PARTICLES_PER_CELL+next]=nextId;
                        id[nextId] = std::pair(*it,i);

                        lastElement = nextId;
                        lastElementInCell[h] = next;
                        notEmpty[h] = true;
                    }
                }
            }

            o++;
        }
    }

    void CellList::cellCollisions(
        uint64_t a1,
        uint64_t b1,
        uint64_t a2,
        uint64_t b2,
        ComponentArray<cCollideable> & dataC,
        ComponentArray<cPhysics> & dataP,
        CollisionResolver * resolver
    )
    {
        if (a1 < 0 || a1 >= rootNCells || b1 < 0 || b2 >= rootNCells || a2 < 0 || a2 >= rootNCells || b2 < 0 || b2 >= rootNCells){
            return;
        }

        if (!notEmpty[a1*rootNCells+b1] || !notEmpty[a2*rootNCells+b2])
        {
            return;
        }

        uint64_t c1 = (a1*rootNCells+b1)*MAX_PARTICLES_PER_CELL;
        uint64_t c2 = (a2*rootNCells+b2)*MAX_PARTICLES_PER_CELL;

        uint64_t p1 = 0;
        uint64_t p2 = 0;
        uint64_t i, j;

        uint64_t n1 = 0;
        uint64_t n2 = 0;

        while(cells[c1+p1] != NULL_INDEX)
        {
            n1++;
            p1++;
        }
        while(cells[c2+p2] != NULL_INDEX)
        {
            n2++;
            p2++;
        }

        p1 = 0; p2 = 0;

        while (p1 < n1)
        {
            p2 = 0;
            i = cells[c1+p1]; 
            auto idi = id[i];
            cCollideable & collidableI = dataC.get(idi.first);
            cPhysics & physicsI = dataP.get(idi.first);
            while (p2 < n2)
            {
                j = cells[c2+p2];
                auto idj = id[j];
                cCollideable & collidableJ = dataC.get(idj.first);
                cPhysics & physicsJ = dataP.get(idj.first);

                resolver->handleObjectObjectCollision(
                    idi.first,idi.second,
                    idj.first,idj.second,
                    collidableI, collidableJ,
                    physicsI, physicsJ
                );
                p2++;
            }
            p1++;
        }
    }

    void CellList::cellCollisionsThreaded(
        ComponentArray<cCollideable> & dataC,
        ComponentArray<cPhysics> & dataP,
        CollisionResolver * resolver,
        std::pair<unsigned,unsigned> * jobs,
        unsigned njobs
    )
    {
        unsigned a, b, a1, b1;

        for (unsigned j = 0; j < njobs; j++)
        {
            a = jobs[j].first;
            b = jobs[j].second;
            a1 = a+1;
            b1 = b+1;

            // takes advantage of symmetry
            //  i.e cell a-1,b-1 will collide with
            //  cell a,b so no need to double up!
            
            cellCollisions(a,b,a,b,dataC,dataP,resolver);
            cellCollisions(a,b,a1,b1,dataC,dataP,resolver);
            cellCollisions(a,b,a,b1,dataC,dataP,resolver);
            cellCollisions(a,b,a1,b,dataC,dataP,resolver);
            cellCollisions(a,b,a1,b-1,dataC,dataP,resolver);
        }
    }

    void CellList::handleObjectObjectCollisions(
        ComponentArray<cCollideable> & dataC,
        ComponentArray<cPhysics> & dataP,
        CollisionResolver * resolver,
        std::set<Id> objects,
        ThreadPool * workers
    )
    {
        populate(dataC, dataP ,objects);
   
        int a1, b1;

        if (workers != nullptr){

            unsigned thread = 0;
            unsigned nThreads = workers->size();

            unsigned jobsPerThread = std::floor(nCells / nThreads);
            std::pair<unsigned,unsigned> threadJobs[nThreads][jobsPerThread];
            std::pair<unsigned,unsigned> jobs[nCells];

            unsigned k = 0;
            for (unsigned i = 0; i < rootNCells; i++){
                for (unsigned j = 0; j < rootNCells; j++)
                {
                    jobs[k] = std::pair<unsigned,unsigned>(i,j);
                    k++;
                }
            }

            for (unsigned t = 0; t < nThreads; t++)
            {
                unsigned k = 0;
                if (t % 2 == 0)
                {
                    for (unsigned j = t*jobsPerThread; j < (t+1)*jobsPerThread; j++)
                    {
                        threadJobs[t][k] = jobs[j];
                        k++;
                    }
                }
                else
                {
                    for (unsigned j = (t+1)*jobsPerThread-1; j > t*jobsPerThread; j--)
                    {
                        threadJobs[t][k] = jobs[j];
                        k++;
                    }  
                }
            }
            //high_resolution_clock::time_point t2 = high_resolution_clock::now();


            for (int t = 0; t < nThreads; t++)
            {
                workers->queueJob(
                    std::bind(
                        &CellList::cellCollisionsThreaded,
                        this,
                        std::ref(dataC),
                        std::ref(dataP),
                        resolver,
                        &threadJobs[t][0],
                        jobsPerThread
                    )
                );
            }
            workers->wait();
            //high_resolution_clock::time_point t3 = high_resolution_clock::now();

            //std::cout << "Thread collisions: " << duration_cast<duration<double>>(t2-t1).count() << ", " << duration_cast<duration<double>>(t3-t2).count() << "\n";

        }
        else
        {

            for (int a = 0; a < rootNCells; a++)
            {
                a1 = a+1;
                for (int b = 0; b < rootNCells; b++)
                {
                    b1 = b+1;
                    // takes advantage of symmetry
                    //  i.e cell a-1,b-1 will collide with
                    //  cell a,b so no need to double up!
                    
                    cellCollisions(a,b,a,b,dataC,dataP,resolver);
                    cellCollisions(a,b,a1,b1,dataC,dataP,resolver);
                    cellCollisions(a,b,a,b1,dataC,dataP,resolver);
                    cellCollisions(a,b,a1,b,dataC,dataP,resolver);
                    cellCollisions(a,b,a1,b-1,dataC,dataP,resolver);
                }
            }
        }
        
    }

    void CellList::handleObjectWorldCollisions(
        ComponentArray<cCollideable> & dataC,
        ComponentArray<cPhysics> & dataP,
        CollisionResolver * resolver,
        AbstractWorld * world,
        std::set<Id> objects,
        ThreadPool * workers
    )
    {

        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            cCollideable & c = dataC.get(*it);
            cPhysics & p = dataP.get(*it);
            resolver->handleObjectWorldCollision(
                *it,
                c,
                p,
                world
            );
        }
    }
}