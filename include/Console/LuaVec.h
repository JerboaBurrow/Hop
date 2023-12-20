#ifndef LUAVEC
#define LUAVEC

#include <lua.h>
#include <vector>

namespace Hop
{

    struct LuaVec
    {
        LuaVec(){}

        void read(lua_State * lua, int index)
        {
            elements = getNumericLuaTable(lua, index);
        }

        bool read(lua_State * lua, const char * name)
        {
            int returnType = lua_getfield(lua, 1, name);

            if (returnType == LUA_TTABLE)
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

        double & operator [] (size_t i){ return elements[i]; }

        size_t size() const { return elements.size(); }

        operator std::vector<double>() { return elements; }

        std::vector<double> elements;
    };
}

#endif /* LUAVEC */
