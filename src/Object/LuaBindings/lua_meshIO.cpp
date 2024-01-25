#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

#include <Object/entityComponentSystem.h>
#include <Component/cCollideable.h>

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
}