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

void SpringDashpot::handleWorldCollision(
    uint64_t i,
    ObjectManager * manager,
    World * world
){

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
