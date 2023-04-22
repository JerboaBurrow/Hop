#include <Console/lua.h>

namespace Hop
{
   
    std::vector<double> getNumericLuaTable(lua_State * lua, int index)
    {
        unsigned length = lua_rawlen(lua,index);
        std::vector<double> values(length);

        for (unsigned i = 1; i <= length; i++)
        {
            lua_pushnumber(lua,i);
            // replaces number i (key) with the value
            lua_gettable(lua,index);
            values[i-1] = lua_tonumber(lua,-1);
            lua_pop(lua,1);
        }

        return values;
    }

    std::vector< std::vector<double> > getLuaTableOfNumericLuaTable(lua_State * lua, int index)
    {
        unsigned length = lua_rawlen(lua,index);
        std::vector<std::vector<double>> values;

        for (unsigned i = 1; i <= length; i++)
        {
            lua_pushnumber(lua,i);
            lua_gettable(lua,index);

            values.push_back
            (
                // not -1, since stack will be added to
                // in this call, need absolute position
                getNumericLuaTable(lua, lua_gettop(lua))
            );

            lua_pop(lua,1);
        }
        return values;
    }

}