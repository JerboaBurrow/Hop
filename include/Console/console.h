#ifndef CONSOLE_H
#define CONSOLE_H

#include <memory>
#include <string>

extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

namespace Hop
{
    class Console 
    {
        public:

            Console()
            {
                lua = luaL_newstate();
                luaL_openlibs(lua);
            }

            ~Console(){ lua_close(lua); }

            bool runFile(std::string file)
            {
                if (luaIsOk())
                {
                    return luaL_dofile(lua,file.c_str());
                }
                return false;
            }

            bool runString(std::string program)
            {
                if (luaIsOk())
                {
                    return luaL_dostring(lua,program.c_str());
                }
                return false;
            }

            bool luaIsOk(){ return lua_status(lua) == LUA_OK ? true : false; }

        private:

        lua_State * lua;
    };
}

#endif /* CONSOLE_H */
