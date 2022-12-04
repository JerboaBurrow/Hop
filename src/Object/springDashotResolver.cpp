#include <Object/springDashpotResolver.h>
#include <Object/collisionMesh.h>

void SpringDashpot::handleObjectCollision(
    std::string oi, std::string oj,
    uint64_t pi, uint64_t pj,
    ObjectManager * manager
){
    if (oi == oj){return;}

    double ix, iy, jx, jy, rx, ry, dd, rc, meff, kr, kd;

    Object & obi = manager->getObject(oi);
    Object & obj = manager->getObject(oj);

    CollisionVertex ci = obi.getCollisionVertex(pi);
    CollisionVertex cj = obj.getCollisionVertex(pj);

    bool gi, gj;

    gi = obi.isGhost();
    gj = obj.isGhost();

    ix = ci.x;
    iy = ci.y;
    jx = cj.x;
    jy = cj.y;

    rx = jx-ix;
    ry = jy-iy;
    dd = rx*rx+ry*ry;
    rc = ci.r+cj.r;
    if (dd < rc*rc){

        if (gi || gj){manager->collisionCallback(oi,oj);}
        
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
