#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    using Hop::Object::Component::cRenderable;

    int EntityComponentSystem::lua_getColour(lua_State * lua)
    {
        LuaString sid;
        int status = lua_checkArgumentCount(lua, 1, "expected id as argument");
        if (status != LUA_OK) { return status; }

        sid.read(lua, 1);

        Id id(sid.characters);

        const cRenderable & t = getComponent<cRenderable>(id);

        lua_pushnumber(lua, t.colour.r);
        lua_pushnumber(lua, t.colour.g);
        lua_pushnumber(lua, t.colour.b);
        lua_pushnumber(lua, t.colour.a);

        return 4;
    }

    int EntityComponentSystem::lua_setColour(lua_State * lua)
    {
        LuaString sid;
        LuaNumber r, g, b, a;
        int status = lua_checkArgumentCount(lua, 5, "expected id, and r, g, b, a as argument");
        if (status != LUA_OK) { return status; }

        sid.read(lua, 1);

        Id id(sid.characters);

        cRenderable & t = getComponent<cRenderable>(id);

        r.read(lua, 2);
        g.read(lua, 3);
        b.read(lua, 4);
        a.read(lua, 5);

        t.colour.r = r.n;
        t.colour.g = g.n;
        t.colour.b = b.n;
        t.colour.a = a.n;

        return 0;
    }
}