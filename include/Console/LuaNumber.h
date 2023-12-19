#ifndef LUANUMBER_H
#define LUANUMBER_H

#include <lua.h>

namespace Hop
{

    struct LuaNumber
    {
        LuaNumber()
        : n(0.0)
        {}

        void read(lua_State * lua, int index)
        {
            n = lua_tonumber(lua, index);
        }

        bool read(lua_State * lua, const char * name)
        {
            int returnType = lua_getfield(lua, 1, name);

            if (returnType == LUA_TNUMBER)
            {
                read(lua, 2);
                lua_pop(lua,1);
                return true;
            }
            else
            {
                lua_pop(lua,1);
                return false;
            }
        }

        bool operator ==(const double & rhs){ return n == rhs; }

        double n;
    };
}


#endif /* LUANUMBER_H */
