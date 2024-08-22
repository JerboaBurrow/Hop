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

    /**
     * @brief Check the argument count to the lua call.
     * @param lua lua_State.
     * @param expected argument count expected.
     * @param msg Error string pushed if not LUA_OK.
     * @return int return code. LUA_OK if the argument count matches.
     */
    int lua_checkArgumentCount(lua_State * lua, int expected, std::string msg);

}

#endif /* LUA */
