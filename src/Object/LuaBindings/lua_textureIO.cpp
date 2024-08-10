#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    using Hop::Object::Component::cSprite;

    int EntityComponentSystem::lua_setTextureRegion(lua_State * lua)
    {
        LuaString sid;
        LuaNumber tx, ty, lx, ly;

        int n = lua_gettop(lua);
        if (n != 5)
        {
            lua_pushliteral(lua,"expected id, tx, ty, lx, ly as argument");
            return lua_error(lua);
        }

        sid.read(lua, 1);

        Id id(sid.characters);
        cSprite & s = getComponent<cSprite>(id);
        tx.read(lua, 2);
        ty.read(lua, 3);
        lx.read(lua, 4);
        ly.read(lua, 5);

        s.tx = tx;
        s.ty = ty;
        s.lx = lx;
        s.ly = ly;

        return 0;
    }

    int EntityComponentSystem::lua_getTextureRegion(lua_State * lua)
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
        const cSprite & s = getComponent<cSprite>(id);
        lua_pushnumber(lua, s.tx);
        lua_pushnumber(lua, s.ty);
        lua_pushnumber(lua, s.lx);
        lua_pushnumber(lua, s.ly);
        return 4;
    }

    int EntityComponentSystem::lua_setTexturePath(lua_State * lua)
    {
        LuaString sid, path;

        int n = lua_gettop(lua);
        if (n != 2)
        {
            lua_pushliteral(lua,"expected id and path as argument");
            return lua_error(lua);
        }

        sid.read(lua, 1);
        path.read(lua, 2);

        Id id(sid.characters);
        cSprite & s = getComponent<cSprite>(id);
        s.texturePath = path;
        return 0;
    }

    int EntityComponentSystem::lua_getTexturePath(lua_State * lua)
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
        const cSprite & s = getComponent<cSprite>(id);
        lua_pushstring(lua, s.texturePath.c_str());
        return 1;
    }
}
