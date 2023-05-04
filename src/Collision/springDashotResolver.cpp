#include <Collision/springDashpotResolver.h>
#include <chrono>
using namespace std::chrono;

#ifndef ANDROID
#else
  #include <android/log.h>
#endif

/* TODO 

    X deprecate LineSegment

    X Rectangle - world vertices update

    Forces

        X Use p-axis to cull?
            Then test each vertex and apply
            forces based upon Box SDF

            Apply forces at point (for rotation)

        Rect-Rect force
        Rect-Circle force
        Rect-world (Rect) force
*/

namespace Hop::System::Physics
{

    using Hop::Maths::pointLineSegmentDistanceSquared;
    using Hop::Maths::pointLineHandedness;
    using Hop::Maths::rectangleRectangleCollided;
    using Hop::Maths::sdf;

    void SpringDashpot::springDashpotSDFForce
    (
        cPhysics & pI, cPhysics & pJ,
        double sd, double nx, double ny,
        double px, double py
    )
    {
        double mag, fx, fy, dinv, vrx, vry, ddot, d, rx, ry;

        mag = 0.0;
        fx = nx;
        fy = ny;

        d = -d;

        d = 10.0*d;

        d *= d;

        mag -= kr*d;

        vrx = pI.vx-pJ.vx;
        vry = pI.vy-pJ.vy;

        // nxt = ny;
        // nyt = -nx;
        ddot = nx*vrx+ny*vry;

        // vnorm = vrx*vrx+vry*vry;

        // if ( (-nxt*vrx-nyt*vry) < (nxt*vrx+nyt*vry) )
        // {
        //     nxt *= -1.0;
        //     nyt *= -1.0;
        // }

        mag -= kd*ddot*d;

        fx *= mag;//+friction*std::abs(mag)*nxt;
        fy *= mag;//+friction*std::abs(mag)*nyt;

        pI.fx += fx;
        pI.fy += fy;

        pI.fpx += px;
        pI.fpy += py;
        pI.fc += 1;

        pJ.fx -= fx;
        pJ.fy -= fy;

        pJ.fpx += px;
        pJ.fpy += py;
        pJ.fc += 1;
    }

    void SpringDashpot::springDashpotForce
    (
        cPhysics & pI, cPhysics & pJ,
        double dd, double rx, double ry, double rc,
        double pxi, double pyi, double pxj, double pyj
    )
    {
        double mag, fx, fy, dinv, nx, ny, vrx, vry, ddot, d;

        mag = 0.0;
        d = std::sqrt(dd);
        dinv = 1.0 / d;
        nx = rx * dinv;
        fx = nx;
        ny = ry * dinv;
        fy = ny;
        dinv = std::min(3.0,dinv);

        mag -= kr*(rc-d)*dinv;

        vrx = pI.vx-pJ.vx;
        vry = pI.vy-pJ.vy;

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

        fx *= mag;//+friction*std::abs(mag)*nxt;
        fy *= mag;//+friction*std::abs(mag)*nyt;


        pI.fx += fx;
        pI.fy += fy;

        pI.fpx += pxi;
        pI.fpy += pyi;
        pI.fc += 1;

        pJ.fx -= fx;
        pJ.fy -= fy;

        pJ.fpx += pxj;
        pJ.fpy += pyj;
        pJ.fc += 1;
    }

     void SpringDashpot::springDashpotWallForce
    (
        double nx,
        double ny,
        double d2,
        double r,
        double px, double py,
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

        dataP.fpx += px;
        dataP.fpy += py;
        dataP.fc += 1;
        
    }

    void SpringDashpot::collisionForce
    (
        cPhysics & pI, cPhysics & pJ,
        Rectangle * li,
        Rectangle * lj
    )
    {
        double nx, ny, s, d;

        bool collided = rectangleRectangleCollided<double>(li,lj,nx,ny,s);

        if (!collided)
        {
            return;
        }

        // must have collided, check forces per vertex

        // li's vertices
        d = sdf(lj,li->llx,li->lly);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, li->llx, li->lly);
        }

        d = sdf(lj,li->ulx,li->uly);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, li->ulx,li->uly);
        }

        d = sdf(lj,li->urx,li->ury);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, li->urx,li->ury);
        }

        d = sdf(li,li->lrx,li->lry);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, li->lrx,li->lry);
        }

        // lj's vertices

        d = sdf(li,lj->llx,lj->lly);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, lj->llx, lj->lly);
        }

        d = sdf(li,lj->ulx,lj->uly);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, lj->ulx,lj->uly);
        }

        d = sdf(li,lj->urx,lj->ury);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, lj->urx,lj->ury);
        }

        d = sdf(li,lj->lrx,lj->lry);

        if (d < 0)
        {
            springDashpotSDFForce(pI, pJ, d, nx, ny, lj->lrx,lj->lry);
        }
    }

    void SpringDashpot::collisionForce
    (
        cPhysics & pI, cPhysics & pJ,
        CollisionPrimitive * c,
        Rectangle * l,
        double rx, double ry, double rc, double dd
    )
    {
        double contactX, contactY, tc;
        double fx, fy, nx, ny, d, dinv, vrx, vry, ddot, mag, px, py;
        bool colliding = false;

        dd = pointLineSegmentDistanceSquared<double>
        (
            c->x,c->y,
            l->llx,l->lly,
            l->ulx,l->uly,
            contactX, contactY
        );

        d = dd; nx = contactX-c->x; ny = contactY-c->y;
        px = contactX; py = contactY;

        dd = pointLineSegmentDistanceSquared<double>
        (
            c->x,c->y,
            l->ulx,l->uly,
            l->urx,l->ury,
            contactX, contactY
        );

        if (dd < d)
        {
            d = dd; nx = contactX-c->x; ny = contactY-c->y;
            px = contactX; py = contactY;
        }

        dd = pointLineSegmentDistanceSquared<double>
        (
            c->x,c->y,
            l->urx,l->ury,
            l->lrx,l->lry,
            contactX, contactY
        );

        if (dd < d)
        {
            d = dd; nx = contactX-c->x; ny = contactY-c->y;
            px = contactX; py = contactY;
        }

        dd = pointLineSegmentDistanceSquared<double>
        (
            c->x,c->y,
            l->lrx,l->lry,
            l->llx,l->lly,
            contactX, contactY
        );

        if (dd < d)
        {
            d = dd; nx = contactX-c->x; ny = contactY-c->y;
            px = contactX; py = contactY;
        }

        if (d < c->r)
        {
            springDashpotForce(pI,pJ,d*d,nx,ny,c->r,px,py,px,py);
        }
    }

    /*
        Circle - Circle collision is easy!
    */
    void SpringDashpot::collisionForce
    (
        cPhysics & pI, cPhysics & pJ,
        CollisionPrimitive * c,
        CollisionPrimitive * l,
        double rx, double ry, double rc, double dd
    )
    {
        double nx, ny;

        nx = l->x-c->x;
        ny = l->y-c->y;

        springDashpotForce(pI,pJ,dd,nx,ny,rc,c->x,c->y,l->x,l->y);

    }

    void SpringDashpot::handleObjectObjectCollision(
        Id & objectI, uint64_t particleI,
        Id & objectJ, uint64_t particleJ,
        cCollideable & cI, cCollideable & cJ,
        cPhysics & pI, cPhysics & pJ
    )
    {

        // the branched code appears marginally faster (avoids? )

        //float objectsNotEqual = !(objectI == objectJ);
        if (objectI == objectJ){return;}

        //high_resolution_clock::time_point t1 = high_resolution_clock::now();


        double ix, iy, jx, jy, rx, ry, rc, dd;

        Rectangle * li, * lj;

        rx = 0.0; ry = 0.0; rc = 0.0;

        std::shared_ptr<CollisionPrimitive> ci = cI.mesh[particleI];
        rx -= ci->x;
        ry -= ci->y;
        rc += ci->r;

        std::shared_ptr<CollisionPrimitive> cj = cJ.mesh[particleJ];
        rx += cj->x;
        ry += cj->y;
        rc += cj->r;

        dd = rx*rx+ry*ry;

        //high_resolution_clock::time_point t2 = high_resolution_clock::now();

        //float isContact = dd < rc*rc;
        if (dd < rc*rc)
        {
            li = dynamic_cast<Rectangle*>(ci.get());
            lj = dynamic_cast<Rectangle*>(cj.get());

            bool iIsRectangle = li != nullptr;
            bool jIsRectangle = lj != nullptr;

            if (iIsRectangle && jIsRectangle)
            {
                collisionForce(pI,pJ,li,lj);
            }
            else if (iIsRectangle && !jIsRectangle)
            {
                collisionForce(pJ,pI,cj.get(),li,rx,ry,rc,dd);
            }
            else if (!iIsRectangle && !jIsRectangle)
            {
                collisionForce(pI,pJ,ci.get(),cj.get(),rx,ry,rc,dd);
            }
            else if (!iIsRectangle && jIsRectangle)
            {
                collisionForce(pI,pJ,ci.get(),lj,rx,ry,rc,dd);
            }
        }
    }

    void SpringDashpot::handleObjectWorldCollision(
        Id id,
        cCollideable & dataC,
        cPhysics & dataP,
        AbstractWorld * world
    )
    {
        TileWorld * tw = dynamic_cast<TileWorld*>(world);

        if (tw != nullptr)
        {
            handleObjectWorldCollision(
                id,
                dataC,
                dataP,
                tw
            );
            return;
        }
        
        MarchingWorld * mw = dynamic_cast<MarchingWorld*>(world);

        if (mw != nullptr)
        {
            handleObjectWorldCollision(
                id,
                dataC,
                dataP,
                mw
            );
            return;
        }
    }

    void SpringDashpot::handleObjectWorldCollision(
        Id id,
        cCollideable & dataC,
        cPhysics & dataP,
        TileWorld * world
    )
    {
        double x0, y0, s;
        Tile h;
        double halfS, S, hx, hy, lx, ly;
        bool inside;

        for (unsigned p = 0; p < dataC.mesh.size(); p++)
        {

            std::shared_ptr<CollisionPrimitive> c = dataC.mesh[p];

            TileNeighbourData neighbours;

            world->neighourTileData(
                c->x,
                c->y,
                neighbours,
                h,
                x0,
                y0,
                s
            );

            TileBoundsData tileBounds;

            world->boundsTileData
            (
                c->x,
                c->y,
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

            if (!inside && !c->recentlyInside())
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

    void SpringDashpot::handleObjectWorldCollision(
        Id id,
        cCollideable & dataC,
        cPhysics & dataP,
        MarchingWorld * world
    )
    {
        double x0, y0, s;
        Tile h;
        double halfS, S, hx, hy, lx, ly;
        bool inside;

        for (unsigned p = 0; p < dataC.mesh.size(); p++)
        {

            std::shared_ptr<CollisionPrimitive> c = dataC.mesh[p];

            
            TileNeighbourData neighbours;

            world->neighourTileData(
                c->x,
                c->y,
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
            
            if (!inside && !c->recentlyInside())
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
        std::shared_ptr<CollisionPrimitive> c,
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

        float d = c->x - (tileNeighbours.west.x+s);

        if (d < c->r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
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

        d = tileNeighbours.north.y-c->y;

        if (d < c->r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
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

        d = tileNeighbours.east.x-c->x;

        if (d < c->r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
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

        d = c->y-(tileNeighbours.south.y+s);

        if (d < c->r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER)
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
        std::shared_ptr<CollisionPrimitive> c,
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
        double lx0, lx1, ly0, ly1;
        double l2x0, l2x1, l2y0, l2y1;
        double nx = 0.0;
        double ny = 0.0;
        bool op = false;
        inside = false;
        
        double thresh = c->r;
        double insideThresh = thresh*thresh;
        int handedness, handedness2;

        Rectangle r1, r2;

        if (h == Tile::EMPTY)
        {   
            // no boundaries
            if (c->recentlyInside())
            {
                if (c->x > x0+thresh ||
                    c->x < lx-thresh ||
                    c->y > y0+thresh ||
                    c->y < ly-thresh 
                )
                {
                    c->setRecentlyInside(0);
                }
            }
            return;
        }
        else if (h == Tile::FULL)
        {

            /*
             _____
            x     x
            |     |
            x_____x
            
            */

            double dw, dn, de, ds;
            dw = pointLineSegmentDistanceSquared<double>
            (
                c->x,c->y,
                x0, y0,
                x0, ly
            );

            dn = pointLineSegmentDistanceSquared<double>
            (
                c->x,c->y,
                x0, ly,
                lx, ly
            );

            de = pointLineSegmentDistanceSquared<double>
            (
                c->x,c->y,
                lx, ly,
                lx, y0
            );

            ds = pointLineSegmentDistanceSquared<double>
            (
                c->x,c->y,
                x0, y0,
                lx, ly
            );

            double d = std::min(std::min(dw,dn),std::min(de,ds));

            if (!neighbour)
            {
                inside = true;
                if (d > insideThresh)
                {
                    c->setRecentlyInside(1);
                }
                return;
            }
            


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
                c->x,c->y,
                hx,y0,
                x0,hy
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                x0,hy
            );
            inside = handedness == 1;

            lx0 = hx; ly0 = y0;
            lx1 = x0; ly1 = hy;
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
                c->x,c->y,
                hx,y0,
                x0,hy
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                x0,hy
            );
            inside = handedness == -1;

            lx0 = hx; ly0 = y0;
            lx1 = x0; ly1 = hy;
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
                c->x,c->y,
                hx,y0,
                lx,hy
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                lx,hy
            );
            inside = handedness == -1;

            lx0 = hx; ly0 = y0;
            lx1 = lx; ly1 = hy;
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
                c->x,c->y,
                hx,y0,
                lx,hy
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                lx,hy
            );
            inside = handedness == 1;

            lx0 = hx; ly0 = y0;
            lx1 = lx; ly1 = hy;
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
                c->x,c->y,
                lx,hy,
                hx,ly
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                lx,hy,
                hx,ly
            );
            inside = handedness == -1;

            lx0 = lx; ly0 = hy;
            lx1 = hx; ly1 = ly;
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
                c->x,c->y,
                lx,hy,
                hx,ly
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                lx,hy,
                hx,ly
            );
            inside = handedness == 1;

            lx0 = lx; ly0 = hy;
            lx1 = hx; ly1 = ly;
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
                c->x,c->y,
                x0,hy,
                hx,ly
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                x0,hy,
                hx,ly
            );
            inside = handedness == 1;

            lx0 = x0; ly0 = hy;
            lx1 = hx; ly1 = ly;
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
                c->x,c->y,
                x0,hy,
                hx,ly
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                x0,hy,
                hx,ly
            );
            inside = handedness == -1;

            lx0 = x0; ly0 = hy;
            lx1 = hx; ly1 = ly;
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
                c->x,c->y,
                x0,hy,
                lx,hy
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                x0,hy,
                lx,hy
            );
            inside = handedness == -1;

            lx0 = x0; ly0 = hy;
            lx1 = lx; ly1 = hy;
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
                c->x,c->y,
                x0,hy,
                lx,hy
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                x0,hy,
                lx,hy
            );
            inside = handedness == 1;
            lx0 = x0; ly0 = hy;
            lx1 = lx; ly1 = hy;
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
                c->x,c->y,
                hx,y0,
                hx,ly
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                hx,ly
            );
            inside = handedness == 1;

            lx0 = hx; ly0 = y0;
            lx1 = hx; ly1 = ly;
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
                c->x,c->y,
                hx,y0,
                hx,ly
            );
            handedness = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                hx,ly
            );
            inside = handedness == -1;
            lx0 = hx; ly0 = y0;
            lx1 = hx; ly1 = ly;
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
                c->x,c->y,
                x0,hy,
                hx,ly
            );

            handedness = pointLineHandedness<double>(
                c->x,c->y,
                x0,hy,
                hx,ly
            );
            lx0 = x0; ly0 = hy;
            lx1 = hx; ly1 = ly;

            if (handedness == -1
            )
            {
                insideLeft = true;
            }


            d2p = pointLineSegmentDistanceSquared<double>(
                c->x,c->y,
                hx,y0,
                lx,hy
            );

            handedness2 = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                lx,hy
            );

            l2x0 = hx; l2y0 = y0;
            l2x1 = lx; l2y1 = hy;

            if (insideLeft && handedness2 == 1
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
                c->x,c->y,
                hx,y0,
                x0,hy
            );

            handedness = pointLineHandedness<double>(
                c->x,c->y,
                hx,y0,
                x0,hy
            );

            lx0 = hx; ly0 = y0;
            lx1 = x0; ly1 = hy;

            if (handedness == -1
            )
            {
                insideLeft = true;
            }
            d2p = pointLineSegmentDistanceSquared<double>(
                c->x,c->y,
                lx,hy,
                hx,ly
            );

            handedness2 = pointLineHandedness<double>(
                c->x,c->y,
                lx,hy,
                hx,ly
            );

            l2x0 = lx; l2y0 = hy;
            l2x1 = hx; l2y1 = ly;

            if (insideLeft && handedness2 == 1
            )
            {
                inside = true;
            }
        }

        if (neighbour){inside = false;}

        if (inside){ 
            std::cout << "inside\n";
            if (!op && d2 > insideThresh){ c->setRecentlyInside(1); }
            else{ double d = std::min(d2,d2p); if (d > insideThresh){ c->setRecentlyInside(1);} }
            return; 
        }

        if (c->recentlyInside())
        {
            std::cout << "r-inside\n";
            if (!op)
            {
                if (d2 > insideThresh)
                {
                    c->setRecentlyInside(0);
                }
                else
                {
                    return;
                }
            }
            else
            {
                double d = std::min(d2,d2p);
                if (d > insideThresh)
                {
                    c->setRecentlyInside(0);
                }
                else
                {
                    return;
                }
            }
        }

        double r2 = c->r*c->r;

        std::cout << d2 << ", " << d2p << "\n";

        bool f1 = d2 < r2;
        bool f2 = op && (d2p < r2);

        Rectangle * li = dynamic_cast<Rectangle*>(c.get());

        if (li == nullptr)
        {
            std::cout << "not rectangle\n";
            if (f1)
            {
                springDashpotWallForce(nx,ny,d2,c->r,c->x,c->y,dataP);
            }

            if (f2)
            {
                springDashpotWallForce(-nx,-ny,d2p,c->r,c->x,c->y,dataP);
            }
        }
        else 
        {
            // rect-rect collision
            double nx1, ny1, nx2, ny2, dd, dd1, dd2, tc;
            double fx, fy, nx, ny, d, dinv, vrx, vry, ddot, mag;
            bool colliding = false;
            cPhysics dataTmp(c->x,c->y,c->y);

            if (f1)
            {
                collisionForce(dataP, dataTmp,li,r1);
            }

            if (f2)
            {
                collisionForce(dataP, dataTmp,li,r2);
            }

        }
    }

    void SpringDashpot::tileBoundariesCollision
    (
        std::shared_ptr<CollisionPrimitive> c,
        cPhysics & dataP,
        TileBoundsData & tileBounds
    )
    {
        double nx, ny, d2;
        double r2 = c->r*c->r;

        double lx0, ly0, lx1, ly1;

        bool colliding = false;

        Rectangle r;

        Rectangle * li = dynamic_cast<Rectangle*>(c.get());

        // WEST

        if 
        (
            tileBounds.wx0 != 0 &&
            tileBounds.wx1 != 0 &&
            tileBounds.wy0 != 0 &&
            tileBounds.wy1 != 0
        )
        {
            nx = 1.0; ny = 0.0;
            lx0 = tileBounds.wx0; ly0 = tileBounds.wy0;
            lx1 = tileBounds.wx1; ly1 = tileBounds.wy1;
            colliding = true;
            r
        }

        // NORTH

        if 
        (
            tileBounds.nx0 != 0 &&
            tileBounds.nx1 != 0 &&
            tileBounds.ny0 != 0 &&
            tileBounds.ny1 != 0
        )
        {
            nx = 0.0; ny = -1.0;
            lx0 = tileBounds.nx0; ly0 = tileBounds.ny0;
            lx1 = tileBounds.nx1; ly1 = tileBounds.ny1;
            colliding = true;
        }

        // EAST

        if 
        (
            tileBounds.ex0 != 0 &&
            tileBounds.ex1 != 0 &&
            tileBounds.ey0 != 0 &&
            tileBounds.ey1 != 0
        )
        {
            nx = -1.0; ny = 0.0;
            lx0 = tileBounds.ex0; ly0 = tileBounds.ey0;
            lx1 = tileBounds.ex1; ly1 = tileBounds.ey1;
            colliding = true;
        }

        // SOUTH

        if 
        (
            tileBounds.sx0 != 0 &&
            tileBounds.sx1 != 0 &&
            tileBounds.sy0 != 0 &&
            tileBounds.sy1 != 0
        )
        {
            nx = 0.0; ny = 1.0;
            lx0 = tileBounds.sx0; ly0 = tileBounds.sy0;
            lx1 = tileBounds.sx1; ly1 = tileBounds.sy1;
            colliding = true;
        }

        if (colliding)
        {
            if (li == nullptr)
            {
                d2 = pointLineSegmentDistanceSquared<double>
                (
                    c->x,c->y,
                    lx0, ly0,
                    lx1, ly1
                );

                if (d2 < r2)
                {
                    springDashpotWallForce(nx,ny,d2,c->r,c->x,c->y,dataP);
                }
            }
            else
            {
                cPhysics dataTmp(0.,0.,0.);
                springDashpotForce(dataP, dataTmp, li, &r);
            }
        }
    }
}