#include <Collision/springDashpotResolver.h>
#include <chrono>
using namespace std::chrono;

#ifndef ANDROID
#else
  #include <android/log.h>
#endif

namespace Hop::System::Physics
{

    using Hop::Maths::pointLineSegmentDistanceSquared;
    using Hop::Maths::pointLineHandedness;

    void SpringDashpot::handleObjectObjectCollision(
        Id & objectI, uint64_t particleI,
        Id & objectJ, uint64_t particleJ,
        ComponentArray<cCollideable> & dataC,
        ComponentArray<cPhysics> & dataP
    )
    {

        // the branched code appears marginally faster (avoids? )

        //float objectsNotEqual = !(objectI == objectJ);
        if (objectI == objectJ){return;}

        //high_resolution_clock::time_point t1 = high_resolution_clock::now();


        double ix, iy, jx, jy, rx, ry, dd, rc, d, dinv;
        double nx, ny, nxt, nyt, ddot, mag, vnorm, fx, fy;
        double vrx, vry;

        rx = 0.0; ry = 0.0; rc = 0.0;

        cCollideable & datai = dataC.get(objectI);
        cPhysics & dataPi = dataP.get(objectI);
        CollisionVertex ci = datai.mesh[particleI];
        rx -= ci.x;
        ry -= ci.y;
        rc += ci.r;

        cCollideable & dataj = dataC.get(objectJ);
        cPhysics & dataPj = dataP.get(objectJ);
        CollisionVertex cj = dataj.mesh[particleJ];
        rx += cj.x;
        ry += cj.y;
        rc += cj.r;

        dd = rx*rx+ry*ry;

        //high_resolution_clock::time_point t2 = high_resolution_clock::now();

        //float isContact = dd < rc*rc;
        if (dd < rc*rc)
        {

            mag = 0.0;
        
            d = std::sqrt(dd);
            dinv = 1.0 / d;
            nx = rx * dinv;
            fx = nx;
            ny = ry * dinv;
            fy = ny;
            dinv = std::min(3.0,dinv);

            mag -= kr*(rc-d)*dinv;

            vrx = dataPi.vx-dataPj.vx;
            vry = dataPi.vy-dataPj.vy;

            // nxt = ny;
            // nyt = -nx;
            ddot = nx*vrx+ny*vry;

            // vnorm = vrx*vrx+vry*vry;

            // if ( (-nxt*vrx-nyt*vry) < (nxt*vrx+nyt*vry) )
            // {
            //     nxt *= -1.0;
            //     nyt *= -1.0;
            // }

            mag -= kd*ddot*dinv;

            //mag *= isContact*objectsNotEqual;

            fx *= mag;//+friction*std::abs(mag)*nxt;
            fy *= mag;//+friction*std::abs(mag)*nyt;


            dataPi.fx += fx;
            dataPi.fy += fy;

            
            dataPj.fx -= fx;
            dataPj.fy -= fy;

            //manager->collisionCallback(objectI,objectJ);

            //high_resolution_clock::time_point t3 = high_resolution_clock::now();

            //std::cout << "resolver: " << duration_cast<duration<double>>(t2-t1).count() << ", " << duration_cast<duration<double>>(t3-t2).count() << "\n";

            //std::cout << mag << ", " << isContact << ", " << objectsNotEqual << "\n";
        }
    }

    void SpringDashpot::handleObjectWorldCollisions(
        Id id,
        ObjectManager * manager,
        TileWorld * world
    )
    {
        double x0, y0, s;
        Tile h;
        double halfS, S, hx, hy, lx, ly;
        bool inside;

        cCollideable & data = manager->getComponent<cCollideable>(id);
        cPhysics & dataP = manager->getComponent<cPhysics>(id);

        for (unsigned p = 0; p < data.mesh.size(); p++)
        {

            CollisionVertex c = data.mesh[p];

            if (data.mesh.recentlyInside(p))
            {
                data.mesh.decrementInside(p);
            }
            else
            {
            
                TileNeighbourData neighbours;

                world->neighourTileData(
                    c.x,
                    c.y,
                    neighbours,
                    h,
                    x0,
                    y0,
                    s
                );

                TileBoundsData tileBounds;

                world->boundsTileData
                (
                    c.x,
                    c.y,
                    h,
                    tileBounds,
                    x0,
                    y0,
                    s
                );  

                halfS = double(s)*0.5;
                S = double(s);

                hx = x0+halfS;
                hy = y0+halfS;
                lx = x0+S;
                ly = y0+S;

                tileCollision
                (
                    h,
                    x0,
                    y0,
                    c,
                    dataP,
                    hx,
                    hy,
                    lx,
                    ly,
                    inside
                );

                if(inside){ data.mesh.resetInsideCounter(p); }
                else
                {

                    tileBoundariesCollision
                    (
                        c,
                        dataP,
                        tileBounds
                    );

                    neighbourTilesCollision
                    (
                    c,
                    dataP,
                    neighbours
                    );

                }
            }
        }
    }

    void SpringDashpot::handleObjectWorldCollisions(
        Id id,
        ObjectManager * manager,
        MarchingWorld * world
    )
    {
        double x0, y0, s;
        Tile h;
        double halfS, S, hx, hy, lx, ly;
        bool inside;

        cCollideable & data = manager->getComponent<cCollideable>(id);
        cPhysics & dataP = manager->getComponent<cPhysics>(id);

        for (unsigned p = 0; p < data.mesh.size(); p++)
        {

            const CollisionVertex & c = data.mesh[p];

            if (data.mesh.recentlyInside(p))
            {
                data.mesh.decrementInside(p);
            }
            else
            {
            
                TileNeighbourData neighbours;

                world->neighourTileData(
                    c.x,
                    c.y,
                    neighbours,
                    h,
                    x0,
                    y0,
                    s
                );

                halfS = double(s)*0.5;
                S = double(s);

                hx = x0+halfS;
                hy = y0+halfS;
                lx = x0+S;
                ly = y0+S;
        
                tileCollision
                (
                    h,
                    x0,
                    y0,
                    c,
                    dataP,
                    hx,
                    hy,
                    lx,
                    ly,
                    inside
                );

                if(inside){ data.mesh.resetInsideCounter(p); }
                else
                {
                    neighbourTilesCollision
                    (
                        c,
                        dataP,
                        neighbours
                    );
                }
            }
        }

    }


    void SpringDashpot::updateParameters(
        double tc,
        double cor
    )
    {
        collisionTime = tc;
        coefficientOfRestitution = cor;
        alpha = (std::log(cor) + M_PI*M_PI) / (tc * tc);
        beta = -std::log(cor) / tc;

        kr = EFFECTIVE_MASS*alpha;
        kd = 2.0*EFFECTIVE_MASS*beta;
    }


    void SpringDashpot::neighbourTilesCollision
    (
        const CollisionVertex & c,
        cPhysics & dataP,
        TileNeighbourData & tileNeighbours
    )
    {
        double hy, hx, lx, ly, x, y;
        bool inside;
        float s = tileNeighbours.west.length;
        double halfS = double(s)*0.5;
        double S = double(s);
        // WEST

        float d = c.x - (tileNeighbours.west.x+s);

        if (d < c.r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
        {
            inside = false;
            hx = tileNeighbours.west.x+halfS;
            hy = tileNeighbours.west.y+halfS;
            lx = tileNeighbours.west.x+S;
            ly = tileNeighbours.west.y+S;
            x = tileNeighbours.west.x;
            y = tileNeighbours.west.y;
    
            tileCollision(
                tileNeighbours.west.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                inside,
                true
            );
        }

        // NORTH

        d = tileNeighbours.north.y-c.y;

        if (d < c.r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
        {
            inside = false;
            hx = tileNeighbours.north.x+halfS;
            hy = tileNeighbours.north.y+halfS;
            lx = tileNeighbours.north.x+S;
            ly = tileNeighbours.north.y+S;
            x = tileNeighbours.north.x;
            y = tileNeighbours.north.y;
    
           tileCollision(
                tileNeighbours.north.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                inside,
                true
            );
        }

        // EAST

        d = tileNeighbours.east.x-c.x;

        if (d < c.r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
        {
            inside = false;
            hx = tileNeighbours.east.x+halfS;
            hy = tileNeighbours.east.y+halfS;
            lx = tileNeighbours.east.x+S;
            ly = tileNeighbours.east.y+S;
            x = tileNeighbours.east.x;
            y = tileNeighbours.east.y;
    
           tileCollision(
                tileNeighbours.east.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                inside,
                true
            );
        }

        // SOUTH

        d = c.y-(tileNeighbours.south.y+s);

        if (d < c.r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
        {
            inside = false;
            hx = tileNeighbours.south.x+halfS;
            hy = tileNeighbours.south.y+halfS;
            lx = tileNeighbours.south.x+S;
            ly = tileNeighbours.south.y+S;
            x = tileNeighbours.south.x;
            y = tileNeighbours.south.y;
    
           tileCollision(
                tileNeighbours.south.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                inside,
                true
            );
        }
    }

    /*
        Find the correct normal vector and distance for a possible force
            op indicates also applying the opposing vector in
            cases with two lines, id in (5,10)
    */
    void SpringDashpot::tileCollision
    (
        Tile & h,
        double x0,
        double y0,
        const CollisionVertex & c,
        cPhysics & dataP,
        double & hx,
        double & hy,
        double & lx,
        double & ly ,
        bool & inside,
        bool neighbour
    )
    {
        double d2, d2p;
        double nx = 0.0;
        double ny = 0.0;
        bool op = false;
        inside = false;

        if (h == Tile::EMPTY)
        {   
            // no boundaries
            return;
        }
        else if (h == Tile::FULL)
        {
            if (!neighbour)
            {
                // within, no boundaries
                inside = true;
                return;
            }

            /*
             _____
            x     x
            |     |
            x_____x
            
            */

            double dw, dn, de, ds;
            dw = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                x0, y0,
                x0, ly
            );

            dn = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                x0, ly,
                lx, ly
            );

            de = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                lx, ly,
                lx, y0
            );

            ds = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                x0, y0,
                lx, ly
            );

            if (dw < dn && dw < de && dw < ds)
            {
                d2 = dw;
                nx = -1.0; ny = 0.0;
            }
            else if(dn < dw && dn < de && dn < ds)
            {
                d2 = dn;
                nx = 0.0; ny = 1.0;
            }
            else if(ds < dw && ds < de && ds < dn)
            {
                d2 = ds;
                nx = 0.0; ny = -1.0;
            }
            else if(de < dw && de < dn && de < ds)
            {
                d2 = de;
                nx = 1.0; ny = 0.0;
            }
            else if (dw == dn)
            {
                d2 = d2;
                nx = -1.0; ny = 1.0;
            }
            else if (dn == de)
            {
                d2 = dn;
                nx = 1.0;
                ny = 1.0;
            }
            else if (de == ds)
            {
                d2 = de;
                nx = 1.0;
                ny = -1.0;
            }
            else if (ds == dw)
            {
                d2 = ds;
                nx = -1.0;
                ny = -1.0;
            }
            else
            {
                return;
            }
        }

        if (h == Tile::BOTTOM_LEFT)
        {
            /*
             _____
            |     |
            |\    |
            x_\___|
            
            */
            nx = 1.0; ny = 1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                x0,hy
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                hx,y0,
                x0,hy
            ) == 1;
        }
        else if (h == Tile::EMPTY_BOTTOM_LEFT)
        {
            /*
             _____
            x     x
            |\    |
            |_\___x
            
            */
            nx = -1.0; ny = -1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                x0,hy
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                hx,y0,
                x0,hy
            ) == -1;
        }
        else if (h == Tile::BOTTOM_RIGHT)
        {
            /*
             _____
            |     |
            |    /|
            |___/_x
            
            */
            nx = -1.0; ny = 1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                lx,hy
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                hx,y0,
                lx,hy
            ) == -1;
        }
        else if (h == Tile::EMPTY_BOTTOM_RIGHT)
        {
            /*
             _____
            x     x
            |    /|
            x___/_|
            
            */
            nx = 1.0; ny = -1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                lx,hy
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                hx,y0,
                lx,hy
            ) == 1;
        }
        else if (h == Tile::TOP_RIGHT)
        {
            /*
             _____
            |   \ x
            |    \|
            |_____|
            
            */
            nx = -1.0; ny = -1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                lx,hy,
                hx,ly
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                lx,hy,
                hx,ly
            ) == -1;
        }
        else if (h == Tile::EMPTY_TOP_RIGHT)
        {
            /*
             _____
            x   \ |
            |    \|
            x_____x
            
            */
            nx = 1.0; ny = 1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                lx,hy,
                hx,ly
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                lx,hy,
                hx,ly
            ) == 1;
        }
        else if (h == Tile::TOP_LEFT)
        {
            /*
             _____
            x /   |
            |/    |
            |_____|
            
            */
            nx = 1.0; ny = -1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                x0,hy,
                hx,ly
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                x0,hy,
                hx,ly
            ) == 1;
        }
        else if (h == Tile::EMPTY_TOP_LEFT)
        {
            /*
             _____
            | /   x
            |/    |
            x_____x
            
            */
            nx = -1.0; ny = 1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                x0,hy,
                hx,ly
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                x0,hy,
                hx,ly
            ) == -1;
        }
        else if (h == Tile::BOTTOM_HALF)
        {
            /*
             _____
            |     |
            |-----|
            x_____x
            
            */
            nx = 0.0; ny = 1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                x0,hy,
                lx,hy
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                x0,hy,
                lx,hy
            ) == -1;
        }
        else if (h == Tile::TOP_HALF)
        {
            /*
             _____
            x     x
            |-----|
            |_____|
            
            */
            nx = 0.0; ny = -1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                x0,hy,
                lx,hy
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                x0,hy,
                lx,hy
            ) == 1;
        }
        else if (h == Tile::LEFT_HALF)
        {
            /*
            _____
            x  |  |
            |  |  |
            x__|__|
            
            */
            nx = 1.0; ny = 0.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                hx,ly
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                hx,y0,
                hx,ly
            ) == 1;
        }
        else if (h == Tile::RIGHT_HALF)
        {
            /*
            _____
            |  |  x
            |  |  |
            |__|__x
            
            */
            nx = -1.0; ny = 0.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                hx,ly
            );
            inside = pointLineHandedness<double>(
                c.x,c.y,
                hx,y0,
                hx,ly
            ) == -1;
        }
        else if (h == Tile::BOTTOM_LEFT_AND_TOP_RIGHT)
        {
            /*
             ____ 
            | /   x
            |/   /|
            x__ /_|
            
            */
            bool insideLeft = false; 
            nx = -1.0; ny = 1.0;
            op = true;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                x0,hy,
                hx,ly
            );
            
            if (pointLineHandedness<double>(
                    c.x,c.y,
                    x0,hy,
                    hx,ly
                    ) == -1
            )
            {
                insideLeft = true;
            }


            d2p = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                lx,hy
            );

            if (insideLeft && pointLineHandedness<double>(
                    c.x,c.y,
                    hx,y0,
                    lx,hy
                    ) == 1
            )
            {
                inside = true;
            }
        }
        else if (h == Tile::TOP_LEFT_AND_BOTTOM_RIGHT)
        {
            /*
            ______
            x   \ |
            |\   \|
            |_\___x
            
            */
            bool insideLeft = false;
            nx = -1.0; ny = -1.0;
            op = true;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                x0,hy
            );

            if (pointLineHandedness<double>(
                    c.x,c.y,
                    hx,y0,
                    x0,hy
                    ) == -1
            )
            {
                insideLeft = true;
            }
            d2p = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                lx,hy,
                hx,ly
            );

            if (insideLeft && pointLineHandedness<double>(
                    c.x,c.y,
                    lx,hy,
                    hx,ly
                    ) == 1
            )
            {
                inside = true;
            }
        }

        if (inside){return;}

        double r2 = c.r*c.r;

        bool f1 = d2 < r2;
        bool f2 = op && (d2p < r2);

        if (f1)
        {
            applyForce(nx,ny,d2,c.r,dataP);
        }

        if (f2)
        {
            applyForce(-nx,-ny,d2p,c.r,dataP);
        }
    }

    void SpringDashpot::applyForce
    (
        double nx,
        double ny,
        double d2,
        double r,
        cPhysics & dataP
    )
    {
        double meff, kr, kd, d, vrx, vry, ddot, mag, fx, fy;

        meff = 1.0 / (1.0/PARTICLE_MASS+1.0/(WALL_MASS_MULTIPLIER));
        kr = meff*alpha;
        kd = 2.0*meff*beta;

        vrx = dataP.vx;
        vry = dataP.vy;

        ddot = nx*vrx+ny*vry;

        d = std::sqrt(d2);

        mag = std::min(1.0/d,10.0)*(kr*(r-d)-kd*ddot);

        fx = mag*nx;//+friction*std::abs(mag)*nxt;
        fy = mag*ny;//+friction*std::abs(mag)*nyt;

        dataP.fx += fx;
        dataP.fy += fy;
        
    }

    void SpringDashpot::tileBoundariesCollision
    (
        const CollisionVertex & c,
        cPhysics & dataP,
        TileBoundsData & tileBounds
    )
    {
        double nx, ny, d2;
        double r2 = c.r*c.r;

        // WEST

        if 
        (
            tileBounds.wx0 == 0 &&
            tileBounds.wx1 == 0 &&
            tileBounds.wy0 == 0 &&
            tileBounds.wy1 == 0
        )
        {
            // no collision here
        }
        else
        {
            nx = 1.0; ny = 0.0;
            d2 = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                tileBounds.wx0, tileBounds.wy0,
                tileBounds.wx1, tileBounds.wy1
            );

            if (d2 < r2)
            {
                applyForce(nx,ny,d2,c.r,dataP);
            }
        }

        // NORTH

        if 
        (
            tileBounds.nx0 == 0 &&
            tileBounds.nx1 == 0 &&
            tileBounds.ny0 == 0 &&
            tileBounds.ny1 == 0
        )
        {
            // no collision here
        }
        else
        {
            nx = 0.0; ny = -1.0;
            d2 = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                tileBounds.nx0, tileBounds.ny0,
                tileBounds.nx1, tileBounds.ny1
            );

            if (d2 < r2)
            {
                applyForce(nx,ny,d2,c.r,dataP);
            }
        }

        // EAST

        if 
        (
            tileBounds.ex0 == 0 &&
            tileBounds.ex1 == 0 &&
            tileBounds.ey0 == 0 &&
            tileBounds.ey1 == 0
        )
        {
            // no collision here
        }
        else
        {
            nx = -1.0; ny = 0.0;
            d2 = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                tileBounds.ex0, tileBounds.ey0,
                tileBounds.ex1, tileBounds.ey1
            );

            if (d2 < r2)
            {
                applyForce(nx,ny,d2,c.r,dataP);
            }
        }

        // SOUTH

        if 
        (
            tileBounds.sx0 == 0 &&
            tileBounds.sx1 == 0 &&
            tileBounds.sy0 == 0 &&
            tileBounds.sy1 == 0
        )
        {
            // no collision here
        }
        else
        {
            nx = 0.0; ny = 1.0;
            d2 = pointLineSegmentDistanceSquared<double>
            (
                c.x,c.y,
                tileBounds.sx0, tileBounds.sy0,
                tileBounds.sx1, tileBounds.sy1
            );

            if (d2 < r2)
            {
                applyForce(nx,ny,d2,c.r,dataP);
            }
        }

    }

}