#include <Console/console.h>

namespace Hop
{
    std::string Console::stackTrace("");

    int configure(lua_State * lua)
    {
        int status = lua_checkArgumentCount(lua, 1, "expected table as argument");
        if (status != LUA_OK) { return status; }
        if (!lua_istable(lua, 1))
        {
            lua_pushliteral(lua, "non table argument");
            return lua_error(lua);
        }
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));

        sPhysics * phys = store->physics;
        sCollision * col = store->resolver;

        LuaNumber dt, subSample, cor, sf;

        if (dt.readField(lua, "timeStep"))
        {
            phys->setTimeStep(dt.n);
        }

        if (subSample.readField(lua, "subSample"))
        {
            phys->setSubSamples(subSample.n);
        }

        if (cor.readField(lua, "cofr"))
        {
            col->setCoefRestitution(cor.n);
        }

        if (sf.readField(lua, "surfaceFriction"))
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
        int status = lua_checkArgumentCount(lua, 3, "expected id and force vector, fx, fy as argument");
        if (status != LUA_OK) { return status; }
        LuaNumber fx, fy;
        LuaString sid;

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