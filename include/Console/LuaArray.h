#ifndef LUA_ARRAY
#define LUA_ARRAY

#include <lua.h>
#include <array>

namespace Hop
{

    template <size_t DIM>
    struct LuaArray
    {
        LuaArray()
        {
            elements.fill(0.0);
        }

        void read(lua_State * lua, int index)
        {
            std::vector data = getNumericLuaTable(lua, index);
            if (data.size() != DIM)
            {
                std::string err = "Wrong number of elements to LuaArray<"+std::to_string(DIM)+">, " + std::to_string(data.size());
                lua_pushliteral(lua, "Wrong number of elements to LuaArray");
                lua_error(lua);
                throw std::runtime_error(err);
            }
            else
            {
                // gauranteed the correct size
                std::copy_n(data.begin(), DIM, elements.begin());
            }
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
        
        operator std::array<double, DIM>() { return elements; }

        std::array<double, DIM> elements;
    };
}

#endif /* LUA_ARRAY */
