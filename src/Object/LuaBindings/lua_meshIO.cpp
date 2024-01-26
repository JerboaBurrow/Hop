#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

#include <Object/entityComponentSystem.h>
#include <Component/cCollideable.h>
#include <Maths/rectangle.h>

namespace Hop::Object
{
    int EntityComponentSystem::lua_removeFromMeshByTag(lua_State * lua)
    {
        int n = lua_gettop(lua);

        if (n != 2)
        {
            lua_pushliteral(lua, "expected id and tag as argument");
            return lua_error(lua);
        }

        LuaString sid;
        LuaNumber ltag;

        sid.read(lua, 1);
        ltag.read(lua, 2);

        Id id(sid.characters);
        uint64_t tag(ltag.n);

        if (hasComponent<cCollideable>(id))
        {
            cCollideable & c = getComponent<cCollideable>(id);
            c.mesh.removeByTag(tag);
        }

        return 0;
    }

    int EntityComponentSystem::lua_meshBoundingBox(lua_State * lua)
    {
        int n = lua_gettop(lua);

        if (n != 1)
        {
            lua_pushliteral(lua, "expected id as argument");
            return lua_error(lua);
        }

        LuaString sid;

        sid.read(lua, 1);

        Id id(sid.characters);

        if (hasComponent<cCollideable>(id))
        {
            cCollideable & c = getComponent<cCollideable>(id);
            Hop::Maths::BoundingBox bb = c.mesh.getBoundingBox();

            lua_createtable(lua, 5, 0);
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.ll.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.ll.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "a");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.ul.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.ul.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "b");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.ur.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.ur.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "c");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.lr.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.lr.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "d");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.centre.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.centre.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "centre");

            return 1;
        }

        return 0;
    }

    int EntityComponentSystem::lua_meshBoundingBoxByTag(lua_State * lua)
    {
        int n = lua_gettop(lua);

        if (n != 2)
        {
            lua_pushliteral(lua, "expected id and tag as argument");
            return lua_error(lua);
        }

        LuaString sid;
        LuaNumber ltag;

        sid.read(lua, 1);
        ltag.read(lua, 2);

        Id id(sid.characters);
        uint64_t tag(ltag.n);

        if (hasComponent<cCollideable>(id))
        {
            cCollideable & c = getComponent<cCollideable>(id);
            Hop::Maths::BoundingBox bb = c.mesh.getBoundingBox(tag);

            lua_createtable(lua, 5, 0);
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.ll.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.ll.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "a");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.ul.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.ul.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "b");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.ur.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.ur.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "c");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.lr.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.lr.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "d");
                lua_createtable(lua, 2, 0);
                    lua_pushnumber(lua, bb.centre.x);
                    lua_setfield(lua, -2, "x");
                    lua_pushnumber(lua, bb.centre.y);
                    lua_setfield(lua, -2, "y");
                lua_setfield(lua, -2, "centre");
                
            return 1;
        }

        return 0;
    }
}