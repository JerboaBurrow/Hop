#include <Collision/springDashpotResolver.h>
#include <Collision/collisionMesh.h>
#include <Collision/vertex.h>

void SpringDashpot::handleObjectCollision(
    std::string & oi, std::string & oj,
    uint64_t pi, uint64_t pj,
    ObjectManager * manager
){
    if (oi == oj){return;}

    double ix, iy, jx, jy, rx, ry, dd, rc, meff, kr, kd, d;
    double nx, ny, nxt, nyt, ddot, mag, vnorm, fx, fy;
    double vrx, vry;

    Id idi = manager->idFromHandle(oi);
    Id idj = manager->idFromHandle(oj);

    cCollideable & datai = manager->getComponent<cCollideable>(idi);
    cCollideable & dataj = manager->getComponent<cCollideable>(idj);

    CollisionVertex ci = datai.mesh[pi];
    CollisionVertex cj = dataj.mesh[pj];

    cPhysics & dataPi = manager->getComponent<cPhysics>(idi);
    cPhysics & dataPj = manager->getComponent<cPhysics>(idj);

    ix = ci.x;
    iy = ci.y;
    jx = cj.x;
    jy = cj.y;

    rx = jx-ix;
    ry = jy-iy;
    dd = rx*rx+ry*ry;
    rc = ci.r+cj.r;

    if (dd < rc*rc){
        
        meff = 1.0 / (1.0+1.0);
        kr = meff*alpha;
        kd = 2.0*meff*beta;

        d = std::sqrt(dd);
        nx = rx / d;
        ny = ry / d;

        vrx = dataPi.vx-dataPj.vx;
        vry = dataPi.vy-dataPj.vy;

        nxt = ny;
        nyt = -nx;
        ddot = nx*vrx+ny*vry;

        vnorm = vrx*vrx+vry*vry;

        if ( (-nxt*vrx-nyt*vry) < (nxt*vrx+nyt*vry) ){
            nxt *= -1.0;
            nyt *= -1.0;
        }

        mag = -kr*(rc-d)-kd*ddot;

        fx = mag*nx+friction*std::abs(mag)*nxt;
        fy = mag*ny+friction*std::abs(mag)*nyt;

        dataPi.fx += fx;
        dataPi.fy += fy;
        
        dataPj.fx -= fx;
        dataPj.fy -= fy;

        manager->collisionCallback(idi,idj);
    }
}

template <class T>
T pointLineSegmentDistanceSquared(
    T px, T py,
    T ax, T ay,
    T bx, T by
){

    T rx = bx-ax; T ry = by-ay;

    T length2 = rx*rx+ry*ry;

    T pMINUSaDOTrOVERlength2 = ((px-ax)*rx + (py-ay)*ry)/length2;
    T tx = ax + pMINUSaDOTrOVERlength2 * rx;
    T ty = ay + pMINUSaDOTrOVERlength2 * ry;

    T dx = px-tx;
    T dy = py-ty;

    return dx*dx+dy*dy;
}

void SpringDashpot::handleObjectWorldCollisions(
    Id id,
    ObjectManager * manager,
    World * world
){

    float h, x0, y0, s;
    double nx, ny, d2, d2p, halfS, S, hx, hy, lx, ly;
    bool op;

    cCollideable & data = manager->getComponent<cCollideable>(id);
    cPhysics & dataP = manager->getComponent<cPhysics>(id);

    for (unsigned p = 0; p < data.mesh.size(); p++){

        CollisionVertex c = data.mesh[p];

        world->worldToCellData(
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
        if (h == 0.0 || h == 15){
            // no boundaries, or within
            break;
        }

        if (h == 1.0){
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
        }
        else if (h == 14.0){
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
        }
        else if (h == 2.0){
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
        }
        else if (h == 13.0){
            /*
             _____
            x     x
            |    /|
            x___/_|
            
            */
            nx = 1.0; ny = -1.0;
            d2 = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,0,
                lx,hy
            );
        }
        else if (h == 4.0){
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
        }
        else if (h == 11.0){
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
        }
        else if (h == 8.0){
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
        }
        else if (h == 7.0){
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
        }
        else if (h == 3.0){
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
        }
        else if (h == 12.0){
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
        }
        else if (h == 9.0){
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
        }
        else if (h == 6.0){
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
        }
        else if (h == 5.0){
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
                hx,hy,
                x0,hy
            );
            d2p = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                lx,hy,
                hx,ly
            );
        }
        else if (h == 10.0){
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
            d2p = pointLineSegmentDistanceSquared<double>(
                c.x,c.y,
                hx,y0,
                lx,hy
            );
        }

        double r2 = c.r*c.r;

        double meff, kr, kd, d, vrx, vry, nxt, nyt, ddot, vnorm, mag, fx, fy;

        bool f1 = d2 < r2;
        bool f2 = op && (d2p < r2);

        std::cout << "data: " << h << ", " << S << ", " << x0 << ", " << y0 << ", " << c.r << ", " << c.x << ", " << c.y << ", " << lx << ", " << ly << "\n";

        if (f1 || f2){
            meff = 1.0 / (1.0+1.0);
            kr = meff*alpha;
            kd = 2.0*meff*beta;

            vrx = dataP.vx;
            vry = dataP.vy;

            nxt = ny;
            nyt = -nx;
            ddot = nx*vrx+ny*vry;

            vnorm = vrx*vrx+vry*vry;

            if ( (-nxt*vrx-nyt*vry) < (nxt*vrx+nyt*vry) ){
                nxt *= -1.0;
                nyt *= -1.0;
            }
        }

        if (f1){
            d = std::sqrt(d2);

            mag = -kr*(c.r-d)-kd*ddot;

            fx = mag*nx+friction*std::abs(mag)*nxt;
            fy = mag*ny+friction*std::abs(mag)*nyt;

            dataP.fx += fx;
            dataP.fy += fy;

            //std::cout << "col: " << fx << ", " << fy << "\n";
        }

        if (f2){
            d = std::sqrt(d2p);

            mag = -kr*(c.r-d)-kd*ddot;

            fx = mag*nx+friction*std::abs(mag)*nxt;
            fy = mag*ny+friction*std::abs(mag)*nyt;

            // dataP.fx -= fx;
            // dataP.fy -= fy;
        }
    }


}

void SpringDashpot::updateParameters(
    double tc,
    double cor
){
    collisionTime = tc;
    coefficientOfRestitution = cor;
    alpha = (std::log(cor) + M_PI*M_PI) / (tc * tc);
    beta = -std::log(cor) / tc;
}
