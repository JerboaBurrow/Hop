#include <System/sPhysics.h>

#include <chrono>
using namespace std::chrono;

void sPhysics::update(ObjectManager * m, double dt){
    double dtdt, nx, ny, ntheta;
    double x,y,lastX,lastY,theta,lastTheta,fx,fy,omega,mass,momentOfInertia;
    dtdt = dt*dt;
    size_t start;
    for (auto it = m->idToIndex.begin(); it != m->idToIndex.end(); it++){
        start = it->second*m->physData;
        
        x = m->objectPhysData[start];
        lastX = m->objectPhysData[start+1];

        y = m->objectPhysData[start+2];
        lastY = m->objectPhysData[start+3];
        
        theta = m->objectPhysData[start+4];
        lastTheta = m->objectPhysData[start+5];

        mass = m->objectPhysData[start+10];
        momentOfInertia = m->objectPhysData[start+11];

        fx = m->objectPhysData[start+12];
        fy = m->objectPhysData[start+13];
        omega = m->objectPhysData[start+14];

        nx = 2.0*x-lastX+fx*dtdt/mass;
        ny = 2.0*y-lastY+fy*dtdt/mass;

        m->objectPhysData[start+7] = (nx-lastX)/2.0;
        m->objectPhysData[start+8] = (ny-lastY)/2.0;

        m->objectPhysData[start+1] = x;
        m->objectPhysData[start+3] = y;

        m->objectPhysData[start] = nx;
        m->objectPhysData[start+2] = ny;

        m->objectPhysData[start+12] = 0.0;
        m->objectPhysData[start+13] = 0.0;

        ntheta = 2.0*theta-lastTheta+omega*dtdt/momentOfInertia;

        m->objectPhysData[start+9]= (ntheta-lastTheta)/2.0;
        
        m->objectPhysData[start+5] = theta;
        m->objectPhysData[start+4] = ntheta;

        m->objectPhysData[start+14] = 0.0;
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
    size_t start = m->idToIndex[i]*m->physData;

    m->objectPhysData[start+12] += fx;
    m->objectPhysData[start+13] += fy;

    if (x != m->objectPhysData[start] || y != m->objectPhysData[start+2]){
        double rx = x-m->objectPhysData[start];
        double ry = y-m->objectPhysData[start+2];
        double tau = rx*fy-ry*fx;
        double r2 = rx*rx+ry*ry;
        m->objectPhysData[start+14] -= tau/(m->objectPhysData[start+10]*r2); 
    }
}