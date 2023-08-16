#ifndef CONSOLE_H
#define CONSOLE_H

#include <memory>
#include <Object/entityComponentSystem.h>
#include <World/world.h>
#include <Console/lua.h>
#include <System/sPhysics.h>
#include <System/sCollision.h>
#include <iostream>

namespace Hop
{

    using Hop::Object::EntityComponentSystem;
    using Hop::World::AbstractWorld;
    using Hop::System::Physics::sPhysics;
    using Hop::System::Physics::sCollision;

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

    // register lib

    const luaL_Reg hopLib[] =
    {
        {"loadObject", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_loadObject>},
        {"maxCollisionPrimitiveSize",&dispatchWorld<&AbstractWorld::lua_worldMaxCollisionPrimitiveSize>},
        {"setPhysicsTimeStep",&dispatchsPhysics<&sPhysics::lua_setTimeStep>},
        {"setCoefRestitution",&dispatchsCollision<&sCollision::lua_setCOR>},
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
                runString("print(\"process running\")");
                std::cout << luaStatus() << "\n";
            }

            ~Console(){ lua_close(lua); }

            bool runFile(std::string file)
            {
                if (luaIsOk())
                {
                    lastCommandOrProgram = file;
                    return luaL_dofile(lua,file.c_str());
                }
                return false;
            }

            bool runString(std::string program)
            {
                if (luaIsOk())
                {   lastCommandOrProgram = program;
                    return luaL_dostring(lua,program.c_str());
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

            void luaStore(LuaExtraSpace * ptr)
            {
	            *static_cast<LuaExtraSpace**>(lua_getextraspace(lua)) = ptr;
            }

        private:

        lua_State * lua;

        std::string lastCommandOrProgram;

    };
}

#endif /* CONSOLE_H */
