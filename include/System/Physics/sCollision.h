#ifndef SCOLLISION_H
#define SCOLLISION_H

#include <memory>
#include <System/system.h>

#include <Collision/cellList.h>
#include <Collision/springDashpotResolver.h>

#include <Object/entityComponentSystem.h>
#include <Component/componentArray.h>

#include <chrono>
using namespace std::chrono;
#include <unordered_map>

namespace Hop::System::Physics
{

    using Hop::World::AbstractWorld;
    using Hop::Object::EntityComponentSystem;
    using Hop::Object::Component::ComponentArray;
    using Hop::Object::Component::cPhysics;
    using Hop::Object::Component::cCollideable;

    /*
        System to detect collisions and apply forces
    */

    class sCollision : public System 
    {

    public:

        sCollision(){}

        void update
        (
            EntityComponentSystem * m, 
            AbstractWorld * w,
            ThreadPool * workers = nullptr
        );

        void setDetector(std::unique_ptr<CollisionDetector> d)
        {
            detector = std::move(d);
        }

        void setResolver(std::unique_ptr<CollisionResolver> r)
        {
            resolver = std::move(r);
        }

        CollisionDetector::CollisionType objectHasCollided(Id & id) { return detector->objectHasCollided(id); }

        void centreOn(std::pair<float,float> p){centreOn(p.first,p.second);}

        void centreOn(double x, double y);

        void setCoefRestitution(double cor){ resolver->setCoefRestitution(cor); }
        void setSurfaceFriction(double f) { resolver->setSurfaceFriction(f); }

        int lua_setCOR(lua_State * lua)
        {  
            int n = lua_gettop(lua);
            
            if (n != 1)
            {
                lua_pushliteral(lua, "requires 1 argument, cor");
                return lua_error(lua);
            }

            if (!lua_isnumber(lua, 1))
            {
                lua_pushliteral(lua, "requires a numeric argument for cor");
                return lua_error(lua);
            }

            double cor = lua_tonumber(lua, 1);
            setCoefRestitution(cor);

            return 0;
        }

        int lua_setFriction(lua_State * lua)
        {  
            int n = lua_gettop(lua);
            
            if (n != 1)
            {
                lua_pushliteral(lua, "requires 1 argument, friction");
                return lua_error(lua);
            }

            if (!lua_isnumber(lua, 1))
            {
                lua_pushliteral(lua, "requires a numeric argument for cor");
                return lua_error(lua);
            }

            double f = lua_tonumber(lua, 1);
            setSurfaceFriction(f);

            return 0;
        }

    private:

        std::unique_ptr<CollisionDetector> detector;
        std::unique_ptr<CollisionResolver> resolver;
        
    };

}
#endif /* SCOLLISION_H */
