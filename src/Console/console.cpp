#include <Console/console.h>

namespace Hop
{
    std::string Console::stackTrace("");

    int configure(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        
        sPhysics * phys = store->physics;
        sCollision * col = store->resolver;

        LuaNumber dt, subSample, cor;
   
        int n = lua_gettop(lua);

        if (!lua_istable(lua, 1))
        {
            lua_pushliteral(lua, "non table argument");
            return lua_error(lua);
        }
        else if (n != 1)
        {
            lua_pushliteral(lua, "more than one argument");
            return lua_error(lua);
        }

        if (dt.read(lua, "timeStep"))
        {
            phys->setTimeStep(dt.n);
        }

        if (subSample.read(lua, "subSample"))
        {
            phys->setSubSamples(subSample.n);
        }

        if (cor.read(lua, "cofr"))
        {
            col->setCoefRestitution(cor.n);
        }

        return 0;
    }

    int LoopRoutines::lua_setRoutines(lua_State * lua)
    {
        LuaTable<Routine> loopRoutines;

        int n = lua_gettop(lua);

        if (n != 1)
        {
            lua_pushliteral(lua, "expected 1 table argument in setLoopRoutines");
            return lua_error(lua);
        }

        loopRoutines.read(lua, 1);

        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        LoopRoutines * loop = store->loopRoutines;

        routines = loopRoutines.data;

        return 0;
    }

    int timeMillis(lua_State * lua)
    {
        int64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        lua_pushnumber(lua, millis);
        return 1;
    }

    bool Routine::read(lua_State * lua, const char * name)
    {
        int returnType = lua_getfield(lua, 1, name);
        if (returnType == LUA_TTABLE)
        {
            LuaString name;
            LuaNumber e;

            if (name.read(lua, "file"))
            {
                filename = name.characters;
                std::cout << filename << ", ";
            }
            else
            {
                lua_pop(lua,1);
                return false;
            }

            if (e.read(lua, "every"))
            {
                every = uint16_t(e.n);
                std::cout << every << "\n";
            }
            else
            {
                lua_pop(lua,1);
                return false;
            }

            return true;
        }
        else
        {
            lua_pop(lua,1);
            return false;
        }
    }

    bool Routine::read(lua_State * lua, int index)
    {

        int returnType = lua_getfield(lua, index, "file");

        if (returnType == LUA_TSTRING)
        {
            filename = lua_tostring(lua, index+1);
            lua_pop(lua,1);
        }
        else
        {
            lua_pop(lua, 1);
            return false;
        }

        returnType = lua_getfield(lua, index, "every");

        if (returnType == LUA_TNUMBER)
        {
            every = lua_tonumber(lua, index+1);
            lua_pop(lua,1);
        }
        else
        {
            return false;
            lua_pop(lua, 1);
        }

        return true;

    }

    int lua_applyForce(lua_State * lua)
    {
        LuaNumber fx, fy;
        LuaString sid;

        int n = lua_gettop(lua);

        if (n != 3)
        {
            lua_pushliteral(lua,"expected id and force vector, fx, fy as argument");
            return lua_error(lua);
        }

        sid.read(lua, 1);
        Hop::Object::Id id(sid.characters);

        fx.read(lua, 2);
        fy.read(lua, 3);

        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));

        if (!store->ecs->hasComponent<cPhysics>(id))
        {
            lua_pushliteral(lua, "object has no physics component");
            return lua_error(lua);
        }

        store->physics->applyForce(store->ecs, fx, fy, true);

        return 0;
    }
}