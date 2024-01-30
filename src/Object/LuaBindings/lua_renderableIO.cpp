#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    using Hop::Object::Component::cRenderable;

    int EntityComponentSystem::lua_getColour(lua_State * lua)
    {
        LuaString sid;

        int n = lua_gettop(lua);

        if (n != 1)
        {
            lua_pushliteral(lua,"expected id as argument");
            return lua_error(lua);
        }

        sid.read(lua, 1);

        Id id(sid.characters);

        const cRenderable & t = getComponent<cRenderable>(id);

        lua_pushnumber(lua, t.r);
        lua_pushnumber(lua, t.g);
        lua_pushnumber(lua, t.b);
        lua_pushnumber(lua, t.a);

        return 4;
    }

    int EntityComponentSystem::lua_setColour(lua_State * lua)
    {
        LuaString sid;
        LuaNumber r, g, b, a;

        int n = lua_gettop(lua);

        if (n != 5)
        {
            lua_pushliteral(lua,"expected id, and r, g, b, a as argument");
            return lua_error(lua);
        }

        sid.read(lua, 1);

        Id id(sid.characters);

        cRenderable & t = getComponent<cRenderable>(id);

        r.read(lua, 2);
        g.read(lua, 3);
        b.read(lua, 4);
        a.read(lua, 5);

        t.r = r.n;
        t.g = g.n;
        t.b = b.n;
        t.a = a.n;

        return 0;
    }
}