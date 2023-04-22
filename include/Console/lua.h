#ifndef LUAUTILS_H
#define LUAUTILS_H

#include <vector>
#include <string>

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

#endif /* LUAUTILS_H */
