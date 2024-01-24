#include <Console/console.h>

namespace Hop
{
    std::string Console::stackTrace("");

    int configure(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        
        sPhysics * phys = store->physics;
        sCollision * col = store->resolver;

        LuaNumber dt, subSample, cor, sf;
   
        int n = lua_gettop(lua);

        if (!lua_istable(lua, 1))
        {
            lua_pushliteral(lua, "non table argument");
            return lua_error(lua);
        }
        else if (n != 1)
        {
            lua_pushliteral(lua, "more than one argument");
            return lua_error(lua);
        }

        if (dt.read(lua, "timeStep"))
        {
            phys->setTimeStep(dt.n);
        }

        if (subSample.read(lua, "subSample"))
        {
            phys->setSubSamples(subSample.n);
        }

        if (cor.read(lua, "cofr"))
        {
            col->setCoefRestitution(cor.n);
        }

        if (sf.read(lua, "surfaceFriction"))
        {
            col->setSurfaceFriction(sf.n);
        }

        return 0;
    }

    int timeMillis(lua_State * lua)
    {
        int64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        lua_pushnumber(lua, millis);
        return 1;
    }

    int lua_applyForce(lua_State * lua)
    {
        LuaNumber fx, fy;
        LuaString sid;

        int n = lua_gettop(lua);

        if (n != 3)
        {
            lua_pushliteral(lua,"expected id and force vector, fx, fy as argument");
            return lua_error(lua);
        }

        sid.read(lua, 1);
        Hop::Object::Id id(sid.characters);

        fx.read(lua, 2);
        fy.read(lua, 3);

        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));

        if (!store->ecs->hasComponent<cPhysics>(id))
        {
            lua_pushliteral(lua, "object has no physics component");
            return lua_error(lua);
        }

        store->physics->applyForce(store->ecs, fx, fy, true);

        return 0;
    }
}