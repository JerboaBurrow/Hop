#ifndef CONSOLE_H
#define CONSOLE_H

#include <memory>
#include <string>
#include <Engine/engine.h>

extern "C"
{
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

namespace Hop
{
    class Engine;
    
    using Hop::Engine;

    typedef int (Engine::*member)(lua_State * lua);

    template <member function>
    int dispatch(lua_State * lua)
    {
        Engine * ptr = *static_cast<Engine**>(lua_getextraspace(lua));
        return ((*ptr).*function)(lua);
    }

    const luaL_Reg hopLib[] =
    {
        {NULL, NULL}
    };

    int load_hopLib(lua_State * lua)
    {
        luaL_newlib(lua,hopLib);
        return 1;
    }

    class Console 
    {
        public:

            Console()
            {
                lua = luaL_newstate();
                luaL_openlibs(lua);
                luaL_requiref(lua,"hop",load_hopLib,1);
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
