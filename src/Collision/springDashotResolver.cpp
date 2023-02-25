#include <Collision/springDashpotResolver.h>

void SpringDashpot::handleObjectObjectCollision(
    std::string & objectI, std::string & objectJ,
    uint64_t particleI, uint64_t particleJ,
    ObjectManager * manager
)
{
    if (objectI == objectJ){return;}

    double ix, iy, jx, jy, rx, ry, dd, rc, meff, kr, kd, d, dinv;
    double nx, ny, nxt, nyt, ddot, mag, vnorm, fx, fy;
    double vrx, vry;

    Id idi = manager->idFromHandle(objectI);
    Id idj = manager->idFromHandle(objectJ);

    cCollideable & datai = manager->getComponent<cCollideable>(idi);
    cPhysics & dataPi = manager->getComponent<cPhysics>(idi);
    CollisionVertex ci = datai.mesh[particleI];
    ix = ci.x;
    iy = ci.y;

    cCollideable & dataj = manager->getComponent<cCollideable>(idj);
    cPhysics & dataPj = manager->getComponent<cPhysics>(idj);
    CollisionVertex cj = dataj.mesh[particleJ];
    jx = cj.x;
    jy = cj.y;

    rc = ci.r+cj.r;

    rx = jx-ix;
    ry = jy-iy;
    dd = rx*rx+ry*ry;

    meff = 1.0 / (2.0/PARTICLE_MASS); // mass defined as 1
    kr = meff*alpha;
    kd = 2.0*meff*beta;

    if (dd < rc*rc)
    {
    
        d = std::sqrt(dd);
        dinv = 1.0 / d;
        nx = rx * dinv;
        ny = ry * dinv;

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

        mag = (-kr*(rc-d)-kd*ddot)*std::min(3.0,dinv);

        fx = mag*nx;//+friction*std::abs(mag)*nxt;
        dataPi.fx += fx;
        dataPj.fx -= fx;

        fy = mag*ny;//+friction*std::abs(mag)*nyt;
        dataPi.fy += fy;
        dataPj.fy -= fy;

        manager->collisionCallback(idi,idj);
    }
}

void SpringDashpot::handleObjectWorldCollisions(
    Id id,
    ObjectManager * manager,
    TileWorld * world
)
{

}

void SpringDashpot::handleObjectWorldCollisions(
    Id id,
    ObjectManager * manager,
    MarchingWorld * world
)
{
    float x0, y0, s;
    Tile h;
    double nx, ny, d2, d2p, halfS, S, hx, hy, lx, ly;
    bool op, inside;

    cCollideable & data = manager->getComponent<cCollideable>(id);
    cPhysics & dataP = manager->getComponent<cPhysics>(id);

    for (unsigned p = 0; p < data.mesh.size(); p++)
    {

        CollisionVertex c = data.mesh[p];

        world->worldToTileData(
            c.x,
            c.y,
            h,
            x0,
            y0,
            s
        );

        nx = 0.0;
        ny = 0.0;
        op = false;
        inside = false;

        halfS = double(s)*0.5;
        S = double(s);

        hx = x0+halfS;
        hy = y0+halfS;
        lx = x0+S;
        ly = y0+S;
        /*
            Find the correct normal vector and distance for a possible force
             op indicates also applying the opposing vector in
             cases with two lines, id in (5,10)
        */
        if (h == Tile::EMPTY || h == Tile::FULL)
        {
            // no boundaries, or within, or null
            break;
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
             _____
            |   \ x
            |\   \|
            X_\___|
            
            */
            nx = 1.0; ny = 1.0;
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
                    ) == 1
            )
            {
                inside = true;
            }


            d2p = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                lx,hy,
                hx,ly
            );

            if (!inside && pointLineHandedness<double>(
                    c.x,c.y,
                    lx,hy,
                    hx,ly
                    ) == -1
            )
            {
                inside = true;
            }
        }
        else if (h == Tile::TOP_LEFT_AND_BOTTOM_RIGHT)
        {
            /*
             _____
            x /   |
            |/   /|
            |___/_x
            
            */
            nx = 1.0; ny = -1.0;
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
                    ) == 1
            )
            {
                inside = true;
            }

            d2p = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                lx,hy
            );

            if (!inside && pointLineHandedness<double>(
                    c.x,c.y,
                    hx,y0,
                    lx,hy
                    ) == -1
            )
            {
                inside = true;
            }
        }

        if (inside){continue;}

        double r2 = c.r*c.r;

        double meff, kr, kd, d, vrx, vry, nxt, nyt, ddot, vnorm, mag, fx, fy;

        bool f1 = d2 < r2;
        bool f2 = op && (d2p < r2);

        if (f1 || f2)
        {
            meff = 1.0 / (1.0/PARTICLE_MASS+1.0/(WALL_MASS_MULTIPLIER));
            kr = meff*alpha;
            kd = 2.0*meff*beta;

            vrx = dataP.vx;
            vry = dataP.vy;

            ddot = nx*vrx+ny*vry;
        }

        if (f1)
        {
            d = std::sqrt(d2);

            mag = std::min(1.0/d,10.0)*(kr*(c.r-d)-kd*ddot);

            fx = mag*nx;//+friction*std::abs(mag)*nxt;
            fy = mag*ny;//+friction*std::abs(mag)*nyt;

            dataP.fx += fx;
            dataP.fy += fy;

    
        }

        if (f2)
        {
            d = std::sqrt(d2p);

            mag = kr*(c.r-d);//-kd*ddot;

            fx = mag*nx;//+friction*std::abs(mag)*nxt;
            fy = mag*ny;//+friction*std::abs(mag)*nyt;

            //dataP.fx += fx;
            //dataP.fy += fy;
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
}
