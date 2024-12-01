#ifndef SPHYSICS_H
#define SPHYSICS_H

#include <Object/entityComponentSystem.h>
#include <Component/cPhysics.h>
#include <Maths/special.h>
#include <Component/componentArray.h>
#include <System/Physics/sCollision.h>
#include <World/world.h>
#include <Console/lua.h>

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

    /**
     * @brief Physics system.
     * 
     */
    class sPhysics : public System
    {

    public:

        /**
         * @brief Construct a default physics system.
         * 
         */
        sPhysics()
        : dt(1.0/900.0),
          dtdt(dt*dt),
          gravity(9.81),
          ngx(0.0),
          ngy(-1.0),
          subSamples(1)
        {}

        /**
         * @brief Update objects
         * 
         * @param m object manager.
         * @param collisions optional collision system.
         * @param world optional world.
         */
        void step
        (
            EntityComponentSystem * m,
            sCollision * collisions = nullptr,
            AbstractWorld * world = nullptr
        );

        /**
         * @brief Set the strength and direction of gravity.
         * 
         * @param g magnitude.
         * @param nx direction in x.
         * @param ny direction in y.
         */
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

        /**
         * @brief Apply a force to object i.
         * 
         * @param m manager.
         * @param i object to apply force to.
         * @param fx force in x.
         * @param fy force in y.
         * @param global apply to all mesh components equally.
         */
        void applyForce(
            EntityComponentSystem * m,
            Id & i,
            double fx,
            double fy,
            bool global = false
        );

        /**
         * @brief Apply a torque to object i.
         * 
         * @param m manager.
         * @param i object to apply torque to.
         * @param tau torque.
         */
        void applyTorque(
            EntityComponentSystem * m,
            Id & i,
            double tau
        );

        /**
         * @brief Apply a force to all objects.
         * 
         * @param m manager.
         * @param fx force in x.
         * @param fy force in y.
         * @param global apply to each objects mesh globally.
         */
        void applyForce(
            EntityComponentSystem * m,
            double fx,
            double fy,
            bool global = false
        );

        /**
         * @brief Determine parameters for stable forces (resp. collisions).
         * 
         * @param m 
         */
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

        double kineticEnergy()
        {
            return energy;
        }

        int lua_kineticEnergy(lua_State * lua)
        {
            lua_pushnumber(lua, energy);
            return 1;
        }

        void setTimeStep(double delta){dt = delta; dtdt = dt*dt;}
        double getTimeStep() const { return dt; }

        void setSubSamples(unsigned s){subSamples = s;}
        unsigned getSubSamples() const { return subSamples; }

        void setGravity(double g, double nx, double ny){gravity = g; ngx = nx; ngy = ny;}
        double getGravity() const { return gravity; }
        glm::vec2 getGravityDirection() const { return glm::vec2(ngx, ngy); }

        // Lua

        int lua_setGravity(lua_State * lua);

    private:

        void update(EntityComponentSystem * m, ThreadPool * workers = nullptr);

        void gravityForce
        (
            EntityComponentSystem * m
        );

        std::default_random_engine e;
        std::normal_distribution<double> normal;

        double dt;
        double dtdt;
        double gravity, ngx, ngy;
        double movementLimitRadii = 0.33;
        unsigned subSamples;
        double energy = 0.0;

        // see implementation for details
        double stableDragUnderdampedLangevinWithGravityUnitMass(
            double dt,
            double gravity,
            double radius
        );

    };

}
#endif /* SPHYSICS_H */
