#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    using Hop::Object::Component::cSprite;

    int EntityComponentSystem::lua_setTextureRegion(lua_State * lua)
    {
        int status = lua_checkArgumentCount(lua, 5, "expected id, tx, ty, lx, ly as argument");
        if (status != LUA_OK) { return status; }
        LuaString sid;
        LuaNumber tx, ty, lx, ly;

        sid.read(lua, 1);

        Id id(sid.characters);
        cSprite & s = getComponent<cSprite>(id);
        tx.read(lua, 2);
        ty.read(lua, 3);
        lx.read(lua, 4);
        ly.read(lua, 5);

        s.textureRegion = jGL::TextureRegion(tx, ty, lx, ly);

        return 0;
    }

    int EntityComponentSystem::lua_getTextureRegion(lua_State * lua)
    {
        int status = lua_checkArgumentCount(lua, 1, "expected id as argument");
        if (status != LUA_OK) { return status; }
        LuaString sid;

        sid.read(lua, 1);

        Id id(sid.characters);
        const cSprite & s = getComponent<cSprite>(id);
        lua_pushnumber(lua, s.textureRegion.tx);
        lua_pushnumber(lua, s.textureRegion.ty);
        lua_pushnumber(lua, s.textureRegion.lx);
        lua_pushnumber(lua, s.textureRegion.ly);
        return 4;
    }

    int EntityComponentSystem::lua_setTexturePath(lua_State * lua)
    {
        int status = lua_checkArgumentCount(lua, 2, "expected id and path as argument");
        if (status != LUA_OK) { return status; }
        LuaString sid, path;

        sid.read(lua, 1);
        path.read(lua, 2);

        Id id(sid.characters);
        cSprite & s = getComponent<cSprite>(id);
        s.texturePath = path;
        return 0;
    }

    int EntityComponentSystem::lua_getTexturePath(lua_State * lua)
    {
        int status = lua_checkArgumentCount(lua, 1, "expected id as argument");
        if (status != LUA_OK) { return status; }
        LuaString sid;

        sid.read(lua, 1);

        Id id(sid.characters);
        const cSprite & s = getComponent<cSprite>(id);
        lua_pushstring(lua, s.texturePath.c_str());
        return 1;
    }
}
