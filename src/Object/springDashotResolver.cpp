#include <Object/springDashpotResolver.h>
#include <Object/collisionMesh.h>
#include <Object/vertex.h>

void SpringDashpot::handleObjectCollision(
    std::string oi, std::string oj,
    uint64_t pi, uint64_t pj,
    ObjectManager * manager
){
    if (oi == oj){return;}

    double ix, iy, jx, jy, rx, ry, dd, rc, meff, kr, kd, d;
    double nx, ny, nxt, nyt, ddot, mag, vnorm, fx, fy;

    std::shared_ptr<Object> obi = manager->getObject(oi);
    std::shared_ptr<Object> obj = manager->getObject(oj);

    CollisionVertex ci = obi.get()->getCollisionVertex(pi);
    CollisionVertex cj = obj.get()->getCollisionVertex(pj);

    bool gi, gj;

    gi = obi.get()->isGhost();
    gj = obj.get()->isGhost();

    ix = ci.x;
    iy = ci.y;
    jx = cj.x;
    jy = cj.y;

    rx = jx-ix;
    ry = jy-iy;
    dd = rx*rx+ry*ry;
    rc = ci.r+cj.r;
    if (dd < rc*rc){

        if (gi || gj){manager->collisionCallback(oi,oj); return;}
        
        meff = 1.0 / (1.0/obi.get()->getMass()+1.0/obj.get()->getMass());
        kr = meff*alpha;
        kd = 2.0*meff*beta;
        d = std::sqrt(dd);
        nx = rx / d;
        ny = ry / d;
        Vertex vr = obi.get()->getVelocity(ix,iy)-obj.get()->getVelocity(jx,jy);
        nxt = ny;
        nyt = -nx;
        ddot = nx*vr.x+ny*vr.y;

        vnorm = norm(vr);

        if ( (-nxt*vr.x-nyt*vr.y) < (nxt*vr.x+nyt*vr.y) ){
            nxt *= -1.0;
            nyt *= -1.0;
        }

        mag = -kr*(rc-d)-kd*ddot;

        fx = mag*nx+friction*std::abs(mag)*nxt;
        fy = mag*ny+friction*std::abs(mag)*nyt;

        manager->collisionCallback(oi,oj);
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
