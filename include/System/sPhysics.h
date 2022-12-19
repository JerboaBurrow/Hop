#ifndef SPHYSICS_H
#define SPHYSICS_H

#include <Object/objectManager.h>
#include <Component/cPhysics.h>

class ObjectManager;
class sPhysics;
/*
    System to update cPhysics components given forces
*/
class sPhysics : public System {
public:

    sPhysics(){}

    void update(ObjectManager * m, double dt);
    
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

private:

    void processThreaded(ObjectManager * m, double dtdt, size_t threadId);

    void updateThreaded(ObjectManager * m, double dt);

};

#endif /* SPHYSICS_H */
