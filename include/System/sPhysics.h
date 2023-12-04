#ifndef SPHYSICS_H
#define SPHYSICS_H

#include <Object/entityComponentSystem.h>
#include <Component/cPhysics.h>
#include <Maths/special.h>
#include <Thread/threadPool.h>
#include <Component/componentArray.h>
#include <System/sCollision.h>
#include <World/world.h>

namespace Hop::Object
{
    class EntityComponentSystem;
}


namespace Hop::System::Physics
{

    class sCollision;

    using Hop::Object::EntityComponentSystem;
    using Hop::Object::Component::ComponentArray;
    using Hop::Object::Component::cPhysics;
    using Hop::Object::Component::cCollideable;
    using Hop::Object::Component::cTransform;
    using Hop::System::Physics::sCollision;
    using Hop::World::AbstractWorld;

    /*
        System to update cPhysics components given forces
    */
    class sPhysics : public System 
    {
        
    public:

        sPhysics()
        : dt(1.0/900.0), 
          dtdt(dt*dt),
          gravity(9.81),
          ngx(0.0),
          ngy(-1.0),
          subSamples(1)
        {}

        void step
        (
            EntityComponentSystem * m, 
            sCollision * collisions,
            AbstractWorld * world
        );

        void setGravityForce
        (
            double g,
            double nx,
            double ny
        )
        {
            gravity = g;
            ngx = nx;
            ngy = ny;
        }
        
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
        
        int lua_setTimeStep(lua_State * lua)
        {   
            int n = lua_gettop(lua);
            
            if (n != 1)
            {
                lua_pushliteral(lua, "requires 1 argument, delta");
                return lua_error(lua);
            }

            if (!lua_isnumber(lua, 1))
            {
                lua_pushliteral(lua, "requires a numeric argument for delta");
                return lua_error(lua);
            }

            double delta = lua_tonumber(lua, 1);
            setTimeStep(delta);

            return 0;
        }

        int lua_setSubSamples(lua_State * lua)
        {   
            int n = lua_gettop(lua);
            
            if (n != 1)
            {
                lua_pushliteral(lua, "requires 1 argument, subsamples");
                return lua_error(lua);
            }

            if (!lua_isinteger(lua, 1))
            {
                lua_pushliteral(lua, "requires an integer argument for subsamples");
                return lua_error(lua);
            }

            unsigned subSamples = lua_tointeger(lua, 1);
            setSubSamples(subSamples);

            return 0;
        }
        
        void setTimeStep(double delta){dt = delta; dtdt = dt*dt;}
        void setSubSamples(unsigned s){subSamples = s;}
        void setGravity(double g, double nx, double ny){gravity = g; ngx = nx; ngy = ny;}

    private:

        void update(EntityComponentSystem * m, ThreadPool * workers = nullptr);

        void gravityForce
        (
            EntityComponentSystem * m
        );

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
        double gravity, ngx, ngy;
        double movementLimitRadii = 0.33;
        unsigned subSamples;

        // see implementation for details
        double stableDragUnderdampedLangevinWithGravityUnitMass(
            double dt,
            double gravity,
            double radius
        );

    };

}
#endif /* SPHYSICS_H */
