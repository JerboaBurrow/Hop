#include <System/sPhysics.h>

#include <chrono>
using namespace std::chrono;

#include <thread>

void sPhysics::processThreaded(ObjectManager * m, double dtdt, size_t threadId){
    double nx, ny, ntheta;
    double D = std::sqrt(2.0*0.1*1.0/60.0);
    for (auto it = threadJobs[threadId].begin(); it != threadJobs[threadId].end(); it++){
        cTransform & dataT = m->getComponent<cTransform>(*it);
        cPhysics & dataP = m->getComponent<cPhysics>(*it);

        dataP.fx += 1.0/600.0 * std::cos(dataT.theta)*dataT.scale;
        dataP.fy += 1.0/600.0 * std::sin(dataT.theta)*dataT.scale;
        dataP.omega += D*normal(e);

        nx = 2.0*dataT.x-dataP.lastX+dataP.fx*dtdt/dataP.mass;
        ny = 2.0*dataT.y-dataP.lastY+dataP.fy*dtdt/dataP.mass;

        dataP.vx = (nx-dataP.lastX)/2.0;
        dataP.vy = (ny-dataP.lastY)/2.0;

        dataP.lastX = dataT.x;
        dataP.lastY = dataT.y;

        dataT.x = nx;
        dataT.y = ny;

        dataP.fx = 0.0;
        dataP.fy = 0.0;

        ntheta = 2.0*dataT.theta-dataP.lastTheta+dataP.omega*dtdt/dataP.momentOfInertia;

        dataP.phi = (ntheta-dataP.lastTheta)/2.0;
        
        dataP.lastTheta = dataT.theta;
        dataT.theta = ntheta;

        dataP.omega = 0.0;

        if (m->hasComponent<cCollideable>(*it)){
            cCollideable & data = m->getComponent<cCollideable>(*it);
            data.mesh.updateWorldMesh(
                dataT.x,
                dataT.y,
                dataT.theta,
                dataT.scale
            );
        }
    }
}

void sPhysics::updateThreaded(ObjectManager * m, double dt){
    
    double dtdt = dt*dt;

    for (int j = 0; j < threadJobs.size(); j++){
        m->postJob(
            std::bind(
                &sPhysics::processThreaded,
                this,
                m,
                dtdt,
                j
            )
        );
    }
    m->waitForJobs();
}

void sPhysics::update(ObjectManager * m, double dt){

    if (m->isThreaded()){
        updateThreaded(m,dt);
        return;
    }

    double dtdt, nx, ny, ntheta, ar, br, cr;
    double D = std::sqrt(2.0*0.5*dt);
    unsigned k = 0;
    dtdt = dt*dt;
    for (auto it = objects.begin(); it != objects.end(); it++){
        cTransform & dataT = m->getComponent<cTransform>(*it);
        cPhysics & dataP = m->getComponent<cPhysics>(*it);

        dataP.fx += 1.0/600.0 * std::cos(dataT.theta)*dataT.scale;
        dataP.fy += 1.0/600.0 * std::sin(dataT.theta)*dataT.scale;
        dataP.omega += D*normal(e);

        nx = 2.0*dataT.x-dataP.lastX+dataP.fx*dtdt/dataP.mass;
        ny = 2.0*dataT.y-dataP.lastY+dataP.fy*dtdt/dataP.mass;

        dataP.vx = (nx-dataP.lastX)/2.0;
        dataP.vy = (ny-dataP.lastY)/2.0;

        dataP.lastX = dataT.x;
        dataP.lastY = dataT.y;

        dataP.fx = 0.0;
        dataP.fy = 0.0;

        cr = dt / (2.0*dataP.momentOfInertia);
        br = 1.0/(1.0+cr);
        ar = (1.0-cr)*br;

        D *= normal(e);

        ntheta = 2.0*br*dataT.theta-ar*dataP.lastTheta+br*dataP.omega*dtdt/dataP.momentOfInertia + br*dt/dataP.momentOfInertia * D;

        dataP.phi = (ntheta-dataP.lastTheta)/2.0;

        dataP.lastTheta = dataT.theta;

        dataP.omega = 0.0;

        dataT.x = nx;
        dataT.y = ny;
        dataT.theta = ntheta;

        if (m->hasComponent<cCollideable>(*it)){
            cCollideable & data = m->getComponent<cCollideable>(*it);
            data.mesh.updateWorldMesh(
                dataT.x,
                dataT.y,
                dataT.theta,
                dataT.scale
            );
        }
        
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