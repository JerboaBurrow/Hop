#include <System/sPhysics.h>

void sPhysics::update(ObjectManager * m, double dt){
    for (auto it = objects.begin(); it != objects.end(); it++){
        Id i = *it;

        cTransform & dataT = m->getComponent<cTransform>(i);
        cPhysics & dataP = m->getComponent<cPhysics>(i);

        double dtdt = dt*dt;
        double nx = 2.0*dataT.x-dataP.lastX+dataP.fx*dtdt/dataP.mass;
        double ny = 2.0*dataT.y-dataP.lastY+dataP.fy*dtdt/dataP.mass;

        dataP.vx = (nx-dataP.lastX)/2.0;
        dataP.vy = (ny-dataP.lastY)/2.0;

        dataP.lastX = dataT.x;
        dataP.lastY = dataT.y;

        dataT.x = nx;
        dataT.y = ny;

        dataP.fx = 0.0;
        dataP.fy = 0.0;

        double ntheta = 2.0*dataT.theta-dataP.lastTheta+dataP.omega*dtdt/dataP.momentOfInertia;

        dataP.phi = (ntheta-dataP.lastTheta)/2.0;
        
        dataP.lastTheta = dataT.theta;
        dataT.theta = ntheta;

        dataP.omega = 0.0;
    }
}

void sPhysics::applyForce(
    ObjectManager * m,
    Id i,
    double x,
    double y,
    double fx,
    double fy
){
    cPhysics & dataP = m->getComponent<cPhysics>(i);
    cTransform & dataT = m->getComponent<cTransform>(i);

    dataP.fx += fx;
    dataP.fy += fy;

    if (dataT.x != x || dataT.y != y){
        double rx = x-dataT.x;
        double ry = y-dataT.y;
        double tau = rx*fy-ry*fx;
        double r2 = rx*rx+ry*ry;
        dataP.omega -= tau/(dataP.mass*r2); 
    }
}

void sPhysics::applyForce(
    ObjectManager * m,
    double fx,
    double fy
){
    for (auto it = objects.begin(); it != objects.end(); it++){
        cPhysics & dataP = m->getComponent<cPhysics>(*it);

        dataP.fx += fx;
        dataP.fy += fy;
    }
}