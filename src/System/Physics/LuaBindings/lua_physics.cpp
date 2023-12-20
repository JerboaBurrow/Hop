#include <System/Physics/sPhysics.h>
#include <Console/LuaNumber.h>
#include <Console/LuaString.h>

namespace Hop::System::Physics
{
    int sPhysics::lua_setGravity(lua_State * lua)
    {

        LuaNumber gx, gy;

        int n = lua_gettop(lua);

        if (n != 2)
        {
            lua_pushliteral(lua,"expected gravity vector, gx, gy as argument");
            return lua_error(lua);
        }

        gx.read(lua, 1);
        gy.read(lua, 2);

        double norm = std::sqrt(gx*gx+gy*gy);

        setGravity(norm, double(gx)/norm, double(gy)/norm);

        return 0;

    }

}