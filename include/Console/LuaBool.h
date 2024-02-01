#ifndef LUABOOL
#define LUABOOL

#include <lua.h>

namespace Hop
{

    struct LuaBool
    {
        LuaBool()
        {
            bit = false;
        }

        void read(lua_State * lua, int index)
        {
            bit = lua_toboolean(lua, index);
        }

        bool readField(lua_State * lua, const char * name)
        {
            int returnType = lua_getfield(lua, 1, name);

            if (returnType == LUA_TBOOLEAN)
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

        bool readGlobal(lua_State * lua, const char * name)
        {
            int returnType = lua_getglobal(lua, name);

            if (returnType == LUA_TBOOLEAN)
            {
                read(lua, -1);
                lua_pop(lua,1);
                return true;
            }
            else
            {   
                lua_pop(lua,1);
                return false;
            }
        }


        bool operator ==(const bool & rhs){ return bit == rhs; }
        operator bool() { return bit; }
        bool bit;
    };
}


#endif /* LUABOOL */
