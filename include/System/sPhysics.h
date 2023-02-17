#ifndef SPHYSICS_H
#define SPHYSICS_H

#include <Object/objectManager.h>
#include <Component/cPhysics.h>

class ObjectManager;
class sPhysics;
/*
    System to update cPhysics components given forces
*/
class sPhysics : public System 
{
    
public:

    sPhysics(){dt=1.0/300.0;gravity=9.81;dtdt=dt*dt;}

    void update(ObjectManager * m);
    
    void applyForce(
        ObjectManager * m,
        Id i,
        double x,
        double y,
        double fx,
        double fy
    );

    void applyForce(
        ObjectManager * m,
        double fx,
        double fy
    );

    // automatically compute stable simulation parameters
    // updating all objects
    void stabaliseObjectParameters(ObjectManager * m);
    void setTimeStep(double delta){dt = delta; dtdt = dt*dt;}
    void setGravity(double g){gravity=g;}

private:

    void processThreaded(ObjectManager * m, size_t threadId);

    void updateThreaded(ObjectManager * m);

    std::default_random_engine e;
    std::normal_distribution<double> normal;

    double dt;
    double dtdt;
    double gravity;

    // see implementation for details
    double stableDragUnderdampedLangevinWithGravityUnitMass(
        double dt,
        double gravity,
        double radius
    );

};

#endif /* SPHYSICS_H */
