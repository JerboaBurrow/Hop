#ifndef LUASTRING
#define LUASTRING

#include <lua.h>
#include <string>

namespace Hop
{

    struct LuaString
    {
        LuaString()
        {
            characters = "";
        }

        void read(lua_State * lua, int index)
        {
            characters = lua_tostring(lua, index);
        }

        bool read(lua_State * lua, const char * name)
        {
            int returnType = lua_getfield(lua, 1, name);

            if (returnType == LUA_TSTRING)
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

        bool operator ==(const std::string & rhs){ return characters == rhs; }
        operator std::string() { return characters; }

        std::string characters;
    };
}

#endif /* LUASTRING */
