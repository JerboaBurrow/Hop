#ifndef LUA
#define LUA

#include <vector>
#include <array>
#include <string>
#include <stdexcept>
#include <algorithm>

extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

namespace Hop
{

    std::vector<double> getNumericLuaTable(lua_State * lua, int index);

    std::vector< std::vector<double> > getLuaTableOfNumericLuaTable(lua_State * lua, int index);

}

#endif /* LUA */
