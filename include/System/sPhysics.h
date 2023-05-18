#ifndef SPHYSICS_H
#define SPHYSICS_H

#include <Object/entityComponentSystem.h>

#include <Component/cPhysics.h>

#include <Thread/threadPool.h>

#include <Component/componentArray.h>

namespace Hop::Object
{
    class EntityComponentSystem;
}


namespace Hop::System::Physics
{
    using Hop::Object::EntityComponentSystem;
    using Hop::Object::Component::ComponentArray;
    using Hop::Object::Component::cPhysics;
    using Hop::Object::Component::cCollideable;
    using Hop::Object::Component::cTransform;
    /*
        System to update cPhysics components given forces
    */
    class sPhysics : public System 
    {
        
    public:

        sPhysics(){dt=1.0/300.0;gravity=9.81;dtdt=dt*dt;}

        void update(EntityComponentSystem * m, ThreadPool * workers = nullptr);

        void gravityForce
        (
            EntityComponentSystem * m,
            double g,
            double nx,
            double ny
        );
        
        void applyForce(
            EntityComponentSystem * m,
            Id & i,
            double x,
            double y,
            double fx,
            double fy
        );

        void applyForce(
            EntityComponentSystem * m,
            double fx,
            double fy
        );

        // automatically compute stable simulation parameters
        // updating all objects
        void stabaliseObjectParameters(Hop::Object::EntityComponentSystem * m);
        void setTimeStep(double delta){dt = delta; dtdt = dt*dt;}
        void setGravity(double g){gravity=g;}

    private:

        void processThreaded
        (
            ComponentArray<cCollideable> & collideables,
            ComponentArray<cPhysics> & physics,
            ComponentArray<cTransform> & transforms,
            Id * jobs,
            unsigned njobs
        );

        void updateThreaded(EntityComponentSystem * m,  ThreadPool * workers = nullptr);

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

}
#endif /* SPHYSICS_H */
