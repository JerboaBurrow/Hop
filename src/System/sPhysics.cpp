#include <System/sPhysics.h>

void sPhysics::update(ObjectManager * m, double dt){
    for (auto it = objects.begin(); it != objects.end(); it++){
        Id i = *it;

        cPhysics & data = m->getComponent<cPhysics>(i);
        cRenderable & dataR = m->getComponent<cRenderable>(i);

        double dtdt = dt*dt;
        double nx = 2.0*data.x-data.lastX+data.fx*dtdt/data.mass;
        double ny = 2.0*data.y-data.lastY+data.fy*dtdt/data.mass;

        data.vx = (nx-data.lastX)/2.0;
        data.vy = (ny-data.lastY)/2.0;

        data.lastX = data.x;
        data.lastY = data.y;

        data.x = nx;
        data.y = ny;

        data.fx = 0.0;
        data.fy = 0.0;

        double ntheta = 2.0*data.theta-data.lastTheta+data.omega*dtdt/data.momentOfInertia;

        data.phi = (ntheta-data.lastTheta)/2.0;
        
        data.lastTheta = data.theta;
        data.theta = ntheta;

        data.omega = 0.0;

        dataR.ox = data.x;
        dataR.oy = data.y;
        dataR.otheta = data.theta;
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
    cPhysics & data = m->getComponent<cPhysics>(i);

    data.fx += fx;
    data.fy += fy;

    if (data.x != x || data.y != y){
        double rx = x-data.x;
        double ry = y-data.y;
        double tau = rx*fy-ry*fx;
        double r2 = rx*rx+ry*ry;
        data.omega -= tau/(data.mass*r2); 
    }
}

void sPhysics::applyForce(
    ObjectManager * m,
    double fx,
    double fy
){
    for (auto it = objects.begin(); it != objects.end(); it++){
        cPhysics & data = m->getComponent<cPhysics>(*it);

        data.fx += fx;
        data.fy += fy;
    }
}