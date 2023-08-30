#ifndef CONSOLE_H
#define CONSOLE_H

#include <memory>
#include <Object/entityComponentSystem.h>
#include <World/world.h>
#include <Console/lua.h>
#include <System/sPhysics.h>
#include <System/sCollision.h>
#include <log.h>


namespace Hop
{

    using Hop::Object::EntityComponentSystem;
    using Hop::World::AbstractWorld;
    using Hop::System::Physics::sPhysics;
    using Hop::System::Physics::sCollision;

    using Hop::Logging::INFO;
    using Hop::Logging::WARN;
    using Hop::Logging::ERROR;
    using Hop::Logging::Log;
    using Hop::Logging::ERRORCODE;

    struct LuaExtraSpace
    {
        EntityComponentSystem * ecs;
        AbstractWorld * world;
        sPhysics * physics;
        sCollision * resolver;
    };

    // ECS 

    typedef int (EntityComponentSystem::*EntityComponentSystemMember)(lua_State * lua);

    template <EntityComponentSystemMember function>
    int dispatchEntityComponentSystem(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        EntityComponentSystem * ptr = store->ecs;
        return ((*ptr).*function)(lua);
    }

    // World

    typedef int (AbstractWorld::*AbstractWorldMember)(lua_State * lua);

    template <AbstractWorldMember function>
    int dispatchWorld(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        AbstractWorld * ptr = store->world;
        return ((*ptr).*function)(lua);
    }

    // Physics
    
    typedef int (sPhysics::*sPhysicsMember)(lua_State * lua);

    template <sPhysicsMember function>
    int dispatchsPhysics(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        sPhysics * ptr = store->physics;
        return ((*ptr).*function)(lua);
    }

    // sCollision
    
    typedef int (sCollision::*sCollisionMember)(lua_State * lua);

    template <sCollisionMember function>
    int dispatchsCollision(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        sCollision * ptr = store->resolver;
        return ((*ptr).*function)(lua);
    }

    int configure(lua_State * lua);

    // register lib

    const luaL_Reg hopLib[] =
    {
        {"loadObject", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_loadObject>},
        {"maxCollisionPrimitiveSize",&dispatchWorld<&AbstractWorld::lua_worldMaxCollisionPrimitiveSize>},
        {"setPhysicsTimeStep",&dispatchsPhysics<&sPhysics::lua_setTimeStep>},
        {"setPhysicsSubSamples",&dispatchsPhysics<&sPhysics::lua_setSubSamples>},
        {"setCoefRestitution",&dispatchsCollision<&sCollision::lua_setCOR>},
        {"configure", &configure},
        {NULL, NULL}
    };

    static int load_hopLib(lua_State * lua)
    {
        luaL_newlib(lua,hopLib);
        return 1;
    }

    class Console 
    {
        public:

            Console(Log & l)
            : lastCommandOrProgram(""), lastStatus(false), log(l)
            {
                lua = luaL_newstate();
                luaL_openlibs(lua);
                luaL_requiref(lua,"hop",load_hopLib,1);
                runString("print(\"process running\")");
            }

            ~Console(){ lua_close(lua); }

            bool runFile(std::string file)
            {
                if (luaIsOk())
                {
                    lastCommandOrProgram = file;
                    lastStatus = luaL_loadfile(lua, file.c_str());
                    int epos = lua_gettop(lua);
                    lua_pushcfunction(lua, traceback);
                    lua_insert(lua, epos);
                    lastStatus = lastStatus || lua_pcall(lua, 0, LUA_MULTRET, epos);
                    lua_remove(lua, epos);
                    return handleErrors();
                }
                return false;
            }

            bool runString(std::string program)
            {
                if (luaIsOk())
                {   lastCommandOrProgram = program;
                    lastStatus = luaL_dostring(lua,program.c_str());
                    return handleErrors();
                }
                return false;
            }

            bool luaIsOk(){ return lua_status(lua) == LUA_OK ? true : false; }

            std::string luaStatus()
            {
                int s = lua_status(lua);

                if (s == LUA_OK){return "LUA_OK";}

                std::string status = lastCommandOrProgram + " | ";

                switch(s)
                {
                    case LUA_YIELD:
                        status += "LUA_YIELD";
                        break;
                    case LUA_ERRRUN:
                        status += "LUA_ERRRUN";
                        break;
                    case LUA_ERRSYNTAX:
                        status += "LUA_ERRSYNTAX";
                        break;
                    case LUA_ERRMEM:
                        status += "LUA_ERRMEM";
                        break;
                    case LUA_ERRERR:
                        status += "LUA_ERRERR";
                        break;
                    default:
                        status += "LUA_STATUS_UNKOWN";
                        break;
                }

                return status;
            }

            bool handleErrors()
            {
                if (lastStatus)
                {
                    std::string msg = "Exited with error running "+lastCommandOrProgram+"\n";
                    msg += stackTrace;
                    ERROR(ERRORCODE::LUA, msg) >> log;
                    return true;
                }
                else
                {
                    return false;
                }
            }

            void luaStore(LuaExtraSpace * ptr)
            {
	            *static_cast<LuaExtraSpace**>(lua_getextraspace(lua)) = ptr;
            }

        private:

        lua_State * lua;

        std::string lastCommandOrProgram;
        static std::string stackTrace;
        bool lastStatus;

        Log & log;

        static int traceback(lua_State * lua) {
            if (lua_isstring(lua, -1))
            {
                stackTrace = lua_tostring(lua, -1);
                lua_pop(lua, 1);
            }
            luaL_traceback(lua, lua, NULL, 1);
            stackTrace += std::string("\n") + lua_tostring(lua, -1);
            lua_pop(lua, 1);
            return 0;
        }
    };
}

#endif /* CONSOLE_H */
