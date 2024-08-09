#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    using Hop::System::Physics::cTransform;

    int EntityComponentSystem::lua_getTransform(lua_State * lua)
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

        const cTransform & t = getComponent<cTransform>(id);

        lua_pushnumber(lua, t.x);
        lua_pushnumber(lua, t.y);
        lua_pushnumber(lua, t.theta);
        lua_pushnumber(lua, t.scaleX);
        lua_pushnumber(lua, t.scaleY);

        return 5;

    }

    int EntityComponentSystem::lua_setTransform(lua_State * lua)
    {

        LuaString sid;
        LuaNumber x, y, theta, scaleX, scaleY;

        int n = lua_gettop(lua);

        if (n < 2)
        {
            lua_pushliteral(lua,"expected id and up to x, y, theta, scale (optionally as x and y) as argument");
            return lua_error(lua);
        }

        sid.read(lua, 1);

        Id id(sid.characters);

        cTransform & t = getComponent<cTransform>(id);

        if (n >= 2)
        {
            x.read(lua, 2);
            t.x = x;
        }

        if (n >= 3)
        {
            y.read(lua, 3);
            t.y = y;
        }

        if (n >= 4)
        {
            theta.read(lua, 4);
            t.theta = theta;
        }

        if (n == 5)
        {
            scaleX.read(lua, 5);
            t.scaleX = scaleX;
            t.scaleY = scaleX;
        }

        if (n == 6)
        {
            scaleX.read(lua, 5);
            t.scaleX = scaleX;

            scaleY.read(lua, 6);
            t.scaleY = scaleY;
        }

        if (hasComponent<cPhysics>(id))
        {
            cPhysics & p = getComponent<cPhysics>(id);
            if (n >= 2)
            {
                p.lastX = x;
            }

            if (n >= 3)
            {
                p.lastY = y;
            }

            if (n >= 4)
            {
                p.lastTheta = theta;
            }

            if (hasComponent<cCollideable>(id))
            {
                cCollideable & c = getComponent<cCollideable>(id);
                c.mesh.transform(t);
            }
        }

        return 0;

    }
}