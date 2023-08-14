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
    using Hop::Maths::rectangleRectangleCollided;
    using Hop::Maths::sdf;
    using Hop::Maths::shortestDistanceSquared;
    using Hop::Maths::pointInRectangle;

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

        krR = kr*2.0;
        kdR = kd*2.0;

    }

    void SpringDashpot::springDashpotForce
    (
        cPhysics & pI, cPhysics & pJ,
        double dd, double rx, double ry, double rc,
        double pxi, double pyi, double pxj, double pyj
    )
    {
        double mag, fx, fy, dinv, nx, ny, vrx, vry, ddot, d, dot, tau, nxt, nyt;

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

        ddot = nx*vrx+ny*vry;

        mag -= kd*ddot*dinv;

        nxt = -ny;
        nyt = nx;

        dot = nxt*vrx + nyt*vry;

        if (dot > 0.0){
            nxt = -nxt;
            nyt = -nyt;
        }

        fx *= mag;
        fy *= mag;

        fx += friction*std::abs(mag)*nxt;
        fy += friction*std::abs(mag)*nyt;

        pI.fx += fx;
        pI.fy += fy;

        pJ.fx -= fx;
        pJ.fy -= fy;

        rx = pxi - pI.x;
        ry = pyi - pI.y;

        tau = (rx*fy-ry*fx);

        // this magic non-linearity dampens 
        //  angular oscillations...

        if (std::abs(tau) < 0.01)
        {
            if (tau > 0)
            {
                tau = tau*tau; 
            }
            else
            {
                tau = -tau*tau;
            }
        }

        pI.tau -= tau;

        rx = pxj - pJ.x;
        ry = pyj - pJ.y;

        tau = (rx*fy-ry*fx);

        if (std::abs(tau) < 0.01)
        {
            if (tau > 0)
            {
                tau = tau*tau; 
            }
            else
            {
                tau = -tau*tau;
            }
        }

        pJ.tau += tau;
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
        double meff, kr, kd, d, vrx, vry, ddot, mag, fx, fy, rx, ry, nxt, nyt, dot, tau;

        meff = 1.0 / (1.0/PARTICLE_MASS+1.0/(WALL_MASS_MULTIPLIER));
        kr = meff*alpha;
        kd = 2.0*meff*beta;

        vrx = dataP.vx;
        vry = dataP.vy;

        ddot = nx*vrx+ny*vry;

        d = std::sqrt(d2);

        mag = (kr*(r-d)-kd*ddot)*std::min(1.0/d,3.0);

        nxt = -ny;
        nyt = nx;

        dot = nxt*vrx + nyt*vry;

        if (dot > 0.0){
            nxt = -nxt;
            nyt = -nyt;
        }

        fx = mag*nx;
        fy = mag*ny;

        dataP.fx += fx+0.5*std::abs(mag)*nxt;
        dataP.fy += fy+0.5*std::abs(mag)*nyt;

        rx = px - dataP.x;
        ry = py - dataP.y;

        tau = (rx*fy-ry*fx)/(PARTICLE_MASS);

        // this magic non-linearity dampens 
        //  angular oscillations...

        if (std::abs(tau) < 0.01)
        {
            if (tau > 0)
            {
                tau = tau*tau; 
            }
            else
            {
                tau = -tau*tau;
            }
        }

        dataP.tau -= tau;
        
    }


    void SpringDashpot::springDashpotForce
    (
        cPhysics & pI,
        cPhysics & pJ,
        double od, double nx, double ny,
        double px, double py
    )
    {
        double mag, magC, fx, fy, vrx, vry, ddot, rx, ry, dot, nxt, nyt, tau;

        mag = 0.0;
        magC = 0.0;
        fx = nx;
        fy = ny;

        mag = krR*od;
        magC = mag;

        vrx = pI.vx;
        vry = pI.vy;

        ddot = nx*vrx+ny*vry;

        mag -= kdR*ddot;

        if (mag < 0.0) { return; }

        nxt = -ny;
        nyt = nx;

        dot = nxt*vrx + nyt*vry;

        if (dot > 0.0){
            nxt = -nxt;
            nyt = -nyt;
        }

        fx = fx*mag;
        fy = fy*mag;

        pI.fx += fx+0.5*std::abs(mag)*nxt;
        pI.fy += fy+0.5*std::abs(mag)*nyt;

        pJ.fx -= fx;
        pJ.fy -= fy;

        fx = nx*magC;
        fy = ny*magC;

        rx = px - pI.x;
        ry = py - pI.y;

        tau = (rx*fy-ry*fx);

        // this magic non-linearity dampens 
        //  angular oscillations...

        if (std::abs(tau) < 0.01)
        {
            if (tau > 0)
            {
                tau = tau*tau; 
            }
            else
            {
                tau = -tau*tau;
            }
        }

        pI.tau -= tau;

        rx = px - pJ.x;
        ry = py - pJ.y;

        tau = (rx*fy-ry*fx);

        if (std::abs(tau) < 0.01)
        {
            if (tau > 0)
            {
                tau = tau*tau; 
            }
            else
            {
                tau = -tau*tau;
            }
        }

        pJ.tau += tau;

    }

    void SpringDashpot::collisionForce
    (
        cPhysics & pI, cPhysics & pJ,
        Rectangle * li,
        Rectangle * lj,
        bool wall
    )
    {
        double nx, ny, nxt, nyt, dt, s;
        bool collided = false;

        collided = rectangleRectangleCollided<double>(li,lj,nx,ny,s);

        if (!collided){ return; }

        nxt = lj->x - li->x;
        nyt = lj->y - li->y;

        dt = std::sqrt(nxt*nxt+nyt*nyt);

        nxt /= dt;
        nyt /= dt;
        
        if (nx*nxt+ny*nyt < 0.0)
        {
            nx = -nx;
            ny = -ny;
        }

        if (wall)
        {

            bool sdll = pointInRectangle<double>(li->llx,li->lly,lj);
            bool sdul = pointInRectangle<double>(li->ulx,li->uly,lj);
            bool sdur = pointInRectangle<double>(li->urx,li->ury,lj);
            bool sdlr = pointInRectangle<double>(li->lrx,li->lry,lj);

            unsigned fs = sdll+sdul+sdur+sdlr;

            if (fs == 0)
            {
                springDashpotForce(pI, pJ, s, -nx, -ny, li->x,li->y);
            }
            else
            {

                //nx /= float(fs);
                //ny /= float(fs);

                if (sdll)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->llx,li->lly);
                }
                if (sdul)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->ulx,li->uly);
                }
                if (sdur)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->urx,li->ury);
                }
                if (sdlr)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->lrx,li->lry);
                }
            }

        }
        else
        {
            nxt = nx;
            nyt = ny;

            bool sdll = pointInRectangle<double>(li->llx,li->lly,lj);
            bool sdul = pointInRectangle<double>(li->ulx,li->uly,lj);
            bool sdur = pointInRectangle<double>(li->urx,li->ury,lj);
            bool sdlr = pointInRectangle<double>(li->lrx,li->lry,lj);

            unsigned fs = sdll+sdul+sdur+sdlr;

            if (fs == 0)
            {
                springDashpotForce(pI, pJ, s, -nx, -ny, li->x,li->y);
            }
            else
            {

                //nx /= float(fs);
                //ny /= float(fs);

                if (sdll)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->llx,li->lly);
                }
                if (sdul)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->ulx,li->uly);
                }
                if (sdur)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->urx,li->ury);
                }
                if (sdlr)
                {
                    springDashpotForce(pI, pJ, s, -nx, -ny, li->lrx,li->lry);
                }
            }



            nx = nxt;
            ny = nyt;

            sdll = pointInRectangle<double>(lj->llx,lj->lly,li);
            sdul = pointInRectangle<double>(lj->ulx,lj->uly,li);
            sdur = pointInRectangle<double>(lj->urx,lj->ury,li);
            sdlr = pointInRectangle<double>(lj->lrx,lj->lry,li);

            fs = sdll+sdul+sdur+sdlr;

            if (fs == 0)
            {
                springDashpotForce(pJ, pI, s, nx, ny, lj->x,lj->y);
            }
            else
            {

                //nx /= float(fs);
                //ny /= float(fs);

                if (sdll)
                {
                    springDashpotForce(pJ, pI, s, nx, ny, lj->llx,lj->lly);
                }
                if (sdul)
                {
                    springDashpotForce(pJ, pI, s, nx, ny, lj->ulx,lj->uly);
                }
                if (sdur)
                {
                    springDashpotForce(pJ, pI, s, nx, ny, lj->urx,lj->ury);
                }
                if (sdlr)
                {
                    springDashpotForce(pJ, pI, s, nx, ny, lj->lrx,lj->lry);
                }
            }
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
        // double contactX, contactY, tc;
        // double fx, fy, nx, ny, d, dinv, vrx, vry, ddot, mag, px, py;
        // bool colliding = false;
        double cx, cy, odod, nx, ny, nxt, nyt, dt, d;

        shortestDistanceSquared(c->x, c->y, l, cx, cy, odod);

        d = std::sqrt(odod);
        nx = (cx-c->x)/d;
        ny = (cy-c->y)/d;

        nxt = l->x - c->x;
        nyt = l->y - c->y;

        dt = std::sqrt(nxt*nxt+nyt*nyt);

        nxt /= dt;
        nyt /= dt;
        
        if (nx*nxt+ny*nyt < 0.0)
        {
            nx = -nx;
            ny = -ny;
        }


        if (d < c->r)
        {
            springDashpotForce(pI, pJ, c->r-d, -nx, -ny, cx, cy);
            springDashpotForce(pJ, pI, c->r-d, nx, ny, cx, cy);
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


        double rx, ry, rc, dd;

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
                s,
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
                s,
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
        float ccrit = c->r*NEIGHBOUR_TILE_CHECK_ZONE_MULTIPLIER;
        float ccrit2 = ccrit*ccrit;

        // WEST

        float d = c->x - (tileNeighbours.west.x+s);

        if (d < ccrit)
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
                s,
                inside,
                true
            );
        }

        // NORTH-WEST

        float dx = c->x - (tileNeighbours.northWest.x+s);
        float dy = c->y - (tileNeighbours.northWest.y);
        d = dx*dx+dy*dy;

        if (d < ccrit2)
        {
            inside = false;
            hx = tileNeighbours.northWest.x+halfS;
            hy = tileNeighbours.northWest.y+halfS;
            lx = tileNeighbours.northWest.x+S;
            ly = tileNeighbours.northWest.y+S;
            x = tileNeighbours.northWest.x;
            y = tileNeighbours.northWest.y;
            tileCollision(
                tileNeighbours.northWest.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                s,
                inside,
                true
            );
        }

        // NORTH

        d = tileNeighbours.north.y-c->y;

        if (d < ccrit)
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
                s,
                inside,
                true
            );
        }

        // NORTH-EAST

        dx = c->x - (tileNeighbours.northEast.x);
        dy = c->y - (tileNeighbours.northEast.y);
        d = dx*dx+dy*dy;

        if (d < ccrit2)
        {
            inside = false;
            hx = tileNeighbours.northEast.x+halfS;
            hy = tileNeighbours.northEast.y+halfS;
            lx = tileNeighbours.northEast.x+S;
            ly = tileNeighbours.northEast.y+S;
            x = tileNeighbours.northEast.x;
            y = tileNeighbours.northEast.y;
            tileCollision(
                tileNeighbours.northEast.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                s,
                inside,
                true
            );
        }

        // EAST

        d = tileNeighbours.east.x-c->x;

        if (d < ccrit)
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
                s,
                inside,
                true
            );
        }

        // SOUTH-EAST

        dx = c->x - (tileNeighbours.southEast.x);
        dy = c->y - (tileNeighbours.southEast.y+s);
        d = dx*dx+dy*dy;

        if (d < ccrit2)
        {
            inside = false;
            hx = tileNeighbours.southEast.x+halfS;
            hy = tileNeighbours.southEast.y+halfS;
            lx = tileNeighbours.southEast.x+S;
            ly = tileNeighbours.southEast.y+S;
            x = tileNeighbours.southEast.x;
            y = tileNeighbours.southEast.y;
            tileCollision(
                tileNeighbours.southEast.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                s,
                inside,
                true
            );
        }

        // SOUTH

        d = c->y-(tileNeighbours.south.y+s);

        if (d < ccrit)
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
                s,
                inside,
                true
            );
        }

        // SOUTH-WEST

        dx = c->x - (tileNeighbours.southWest.x+s);
        dy = c->y - (tileNeighbours.southWest.y+s);
        d = dx*dx+dy*dy;

        if (d < ccrit2)
        {
            inside = false;
            hx = tileNeighbours.southWest.x+halfS;
            hy = tileNeighbours.southWest.y+halfS;
            lx = tileNeighbours.southWest.x+S;
            ly = tileNeighbours.southWest.y+S;
            x = tileNeighbours.southWest.x;
            y = tileNeighbours.southWest.y;
            tileCollision(
                tileNeighbours.southWest.tileType,
                x,
                y,
                c,
                dataP,
                hx,
                hy,
                lx,
                ly,
                s,
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
        double & ly,
        double s,
        bool & inside,
        bool neighbour
    )
    {
        double d2, d2p;
        double nx = 0.0;
        double ny = 0.0;
        bool op = false;
        inside = false;
        
        double thresh = c->r;
        double insideThresh = thresh*thresh;
        int handedness, handedness2;

        Rectangle * li = dynamic_cast<Rectangle*>(c.get());

        bool isRectangle = li != nullptr;

        Rectangle r1a, r1b, r1c;

        Rectangle bottomLeft, bottomLeftSquare, bottomRight, bottomRightSquare;
        Rectangle central, topLeft, topLeftSquare, topRight, topRightSquare;
        Rectangle leftHalf, rightHalf, topHalf, bottomHalf;

        bool bc = false;

        if (isRectangle)
        {

            // Rectangle bottomLeft;
            bottomLeft.llx = x0-s/2.0; bottomLeft.lly = y0;
            bottomLeft.ulx = x0;       bottomLeft.uly = y0+s/2.0;
            bottomLeft.urx = x0+s/2.0; bottomLeft.ury = y0;
            bottomLeft.lrx = x0;       bottomLeft.lry = y0-s/2.0;

            // Rectangle bottomLeftSquare;
            bottomLeftSquare.llx = x0;       bottomLeftSquare.lly = y0;
            bottomLeftSquare.ulx = x0;       bottomLeftSquare.uly = y0+s/2.0;
            bottomLeftSquare.urx = x0+s/2.0; bottomLeftSquare.ury = y0+s/2.0;
            bottomLeftSquare.lrx = x0+s/2.0; bottomLeftSquare.lry = y0;

            // Rectangle central;
            central.llx = x0;       central.lly = y0+s/2.0;
            central.ulx = x0+s/2.0; central.uly = y0+s;
            central.urx = x0+s;     central.ury = y0+s/2.0;
            central.lrx = x0+s/2.0; central.lry = y0;

            // Rectangle bottomRight;
            bottomRight.llx = x0+s/2.0;     bottomRight.lly = y0;
            bottomRight.ulx = x0+s;         bottomRight.uly = y0+s/2.0;
            bottomRight.urx = x0+s*3.0/2.0; bottomRight.ury = y0;
            bottomRight.lrx = x0+s;         bottomRight.lry = y0-s/2.0;

            // Rectangle bottomRightSquare;
            bottomRightSquare.llx = x0+s/2.0;     bottomRightSquare.lly = y0;
            bottomRightSquare.ulx = x0+s/2.0;     bottomRightSquare.uly = y0+s/2.0;
            bottomRightSquare.urx = x0+s;         bottomRightSquare.ury = y0+s/2.0;
            bottomRightSquare.lrx = x0+s;         bottomRightSquare.lry = y0;

            // Rectangle topLeft;
            topLeft.llx = x0;           topLeft.lly = y0+s/2.0;
            topLeft.ulx = x0-s/2.0;     topLeft.uly = y0+s;
            topLeft.urx = x0;           topLeft.ury = y0+s*3.0/2.0;
            topLeft.lrx = x0+s/2.0;     topLeft.lry = y0+s;

            // Rectangle topLeftSquare;
            topLeftSquare.llx = x0;           topLeftSquare.lly = y0+s/2.0;
            topLeftSquare.ulx = x0;           topLeftSquare.uly = y0+s;
            topLeftSquare.urx = x0+s/2.0;     topLeftSquare.ury = y0+s;
            topLeftSquare.lrx = x0+s/2.0;     topLeftSquare.lry = y0+s/2.0;

            // Rectangle topRight;
            topRight.llx = x0+s/2.0;     topRight.lly = y0+s;
            topRight.ulx = x0+s;         topRight.uly = y0+s*3.0/2.0;
            topRight.urx = x0+s*3.0/2.0; topRight.ury = y0;
            topRight.lrx = x0+s;         topRight.lry = y0+s/2.0;

            // Rectangle topRightSquare;
            topRightSquare.llx = x0+s/2.0;     topRightSquare.lly = y0+s;
            topRightSquare.ulx = x0+s;         topRightSquare.uly = y0+s;
            topRightSquare.urx = x0+s;         topRightSquare.ury = y0+s/2.0;
            topRightSquare.lrx = x0+s/2.0;     topRightSquare.lry = y0+s/2.0;

            // Rectangle rightHalf;
            rightHalf.llx = x0+s/2.0;     rightHalf.lly = y0;
            rightHalf.ulx = x0+s/2.0;     rightHalf.uly = y0+s;
            rightHalf.urx = x0+s;         rightHalf.ury = y0+s;
            rightHalf.lrx = x0+s;         rightHalf.lry = y0;

            // Rectangle leftHalf;
            leftHalf.llx = x0;           leftHalf.lly = y0;
            leftHalf.ulx = x0;           leftHalf.uly = y0+s;
            leftHalf.urx = x0+s/2.0;     leftHalf.ury = y0+s;
            leftHalf.lrx = x0+s/2.0;     leftHalf.lry = y0;

            // Rectangle bottomHalf;
            bottomHalf.llx = x0;           bottomHalf.lly = y0;
            bottomHalf.ulx = x0;           bottomHalf.uly = y0+s/2.0;
            bottomHalf.urx = x0+s;         bottomHalf.ury = y0+s/2.0;
            bottomHalf.lrx = x0+s;         bottomHalf.lry = y0;

            // Rectangle topHalf;
            topHalf.llx = x0;           topHalf.lly = y0+s/2.0;
            topHalf.ulx = x0;           topHalf.uly = y0+s;
            topHalf.urx = x0+s;         topHalf.ury = y0+s;
            topHalf.lrx = x0+s;         topHalf.lry = y0+s/2.0;

        }

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

            if (isRectangle)
            {
                r1a.llx = x0;    r1a.lly = y0;
                r1a.ulx = x0;    r1a.uly = y0+s;
                r1a.urx = x0+s;  r1a.ury = y0+s;
                r1a.lrx = x0+s;  r1a.lry = y0;
            }

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

            if (isRectangle && !neighbour)
            {
                r1a = bottomLeft;
            }
            else
            {
                r1a = bottomLeftSquare;
            }

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

            if (isRectangle)
            {
                r1a = central;
                r1b = rightHalf;
                r1c = topHalf;
                bc = true;
            }

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

            if (isRectangle && !neighbour)
            {
                r1a = bottomRight;
            }
            else
            {
                r1a = bottomRightSquare;
            }

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


            if (isRectangle)
            {
                r1a = central;
                r1b = leftHalf;
                r1c = topHalf;
                bc = true;
            }

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

            if (isRectangle && !neighbour)
            {
                r1a = topRight;
            }
            else
            {
                r1a = topRightSquare;
            }

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

            if (isRectangle)
            {
                r1a = central;
                r1b = bottomHalf;
                r1c = leftHalf;
                bc = true;
            }

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

            if (isRectangle && !neighbour)
            {
                r1a = topLeft;
            }
            else
            {
                r1a = topLeftSquare;
            }

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

            if (isRectangle)
            {
                r1a = central;
                r1b = bottomHalf;
                r1c = rightHalf;
                bc = true;
            }

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


            if (isRectangle)
            {
                r1a = bottomHalf;
            }

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

            if (isRectangle)
            {
                r1a = topHalf;
            }

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

            if (isRectangle)
            {
                r1a = leftHalf;
            }

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

            if (isRectangle)
            {
                r1a = rightHalf;
            }

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


            if (isRectangle)
            {
                r1a = central;
                
                if (!neighbour)
                {
                    r1b = bottomLeft;
                    r1c = topRight;
                }
                else
                {
                    r1b = bottomLeftSquare;
                    r1c = topRightSquare;
                }

                bc = true;
            }

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


            if (isRectangle)
            {
                r1a = central;

                if (!neighbour)
                {
                    r1b = topLeft;
                    r1c = bottomRight;
                }
                else
                {
                    r1b = topLeftSquare;
                    r1c = bottomRightSquare;                   
                }
                bc = true;
            }

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

            if (insideLeft && handedness2 == 1
            )
            {
                inside = true;
            }
        }

        if (neighbour){inside = false;}


        if (inside){ 
            if (li != nullptr)
            {
                inside = false;
            }
            else
            {
                if (!op && d2 > insideThresh){ c->setRecentlyInside(1); }
                else{ double d = std::min(d2,d2p); if (d > insideThresh){ c->setRecentlyInside(1);} }
                return; 
            }
        }


        if (c->recentlyInside())
        {
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

        double rr = c->r*c->r;

        bool f1 = d2 < rr;
        bool f2 = op && (d2p < rr);

        if (li == nullptr)
        {
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
            cPhysics dataA(c->x,c->y,c->y);
            cPhysics dataB(c->x,c->y,c->y);
            cPhysics dataC(c->x,c->y,c->y);

            r1a.resetAxes();
            r1b.resetAxes();
            r1c.resetAxes();

            if (f1)
            {

                collisionForce(dataP, dataA, li, &r1a, true);

            }

            if ((f1 || f2) && bc)
            {

                collisionForce(dataP, dataB, li, &r1b, true);
                collisionForce(dataP, dataC, li, &r1c, true);

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

            r.llx = lx0-1.0; r.lly = ly0;
            r.ulx = r.llx;   r.uly = ly1;
            r.urx = lx0;     r.ury = ly1;
            r.lrx = lx0;     r.lry = ly0;
            r.resetAxes();
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

            r.llx = lx0;     r.lly = ly0;
            r.ulx = lx0;     r.uly = ly0+1.0;
            r.urx = lx1;     r.ury = ly0+1.0;
            r.lrx = lx1;     r.lry = ly0;
            r.resetAxes();
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

            r.llx = lx0;     r.lly = ly0;
            r.ulx = lx0;     r.uly = ly0+1.0;
            r.urx = lx0+1.0; r.ury = ly0+1.0;
            r.lrx = lx0+1.0; r.lry = ly0;
            r.resetAxes();
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

            r.llx = lx0;     r.lly = ly0;
            r.ulx = lx1;     r.uly = ly0;
            r.urx = lx1;     r.ury = ly0-1.0;
            r.lrx = lx0;     r.lry = ly0-1.0;
            r.resetAxes();
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
                collisionForce(dataP, dataTmp, li, &r, true);
            }
        }
    }
}