#include <Console/console.h>

namespace Hop
{
    std::string Console::stackTrace("");

    int configure(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        
        sPhysics * phys = store->physics;
        sCollision * col = store->resolver;

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

        int returnType = lua_getfield(lua, 1, "timeStep");

        if (returnType != LUA_TNONE && returnType != LUA_TNIL)
        {
            double n = lua_tonumber(lua, 2);
            phys->setTimeStep(n);
        }

        lua_pop(lua, 1);

        returnType = lua_getfield(lua, 1, "subSample");

        if (returnType != LUA_TNONE && returnType != LUA_TNIL)
        {
            unsigned n = lua_tonumber(lua, 2);
            phys->setSubSamples(n);
        }

        lua_pop(lua, 1);

        returnType = lua_getfield(lua, 1, "cofr");

        if (returnType != LUA_TNONE && returnType != LUA_TNIL)
        {
            double n = lua_tonumber(lua, 2);
            col->setCoefRestitution(n);
        }

        lua_pop(lua, 1);

        return 0;
    }

}