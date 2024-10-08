#ifndef CONSOLE_H
#define CONSOLE_H

#include <Object/entityComponentSystem.h>
#include <World/world.h>
#include <Console/lua.h>
#include <Console/LuaNumber.h>
#include <Console/LuaString.h>
#include <Console/LuaTable.h>
#include <Console/LuaBool.h>
#include <System/Physics/sPhysics.h>
#include <System/Physics/sCollision.h>
#include <jLog/jLog.h>
#include <Object/id.h>
#include <Console/scriptz.h>

#include <memory>
#include <vector>
#include <chrono>

namespace Hop
{

    using Hop::Object::EntityComponentSystem;
    using Hop::World::AbstractWorld;
    using Hop::System::Physics::sPhysics;
    using Hop::System::Physics::sCollision;
    using Hop::Object::Component::cPhysics;

    using jLog::INFO;
    using jLog::WARN;
    using jLog::ERR;
    using jLog::Log;
    using jLog::ERRORCODE;

    /**
     * @brief Store for lua global state.
     *  For the console to have access to these classes they must
     *  be set into LuaExtraSpace and set via Console::luaStore.
     */
    struct LuaExtraSpace
    {
        EntityComponentSystem * ecs;
        AbstractWorld * world;
        sPhysics * physics;
        sCollision * resolver;
        Console * console;
        Scriptz * scripts;
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

    // Scriptz

    typedef int (Scriptz::*ScriptzMember)(lua_State * lua);

    template <ScriptzMember function>
    int dispatchScriptz(lua_State * lua)
    {
        LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
        Scriptz * ptr = store->scripts;
        return ((*ptr).*function)(lua);
    }

    /**
     * @brief Configure physics.
     * @remark Takes a lua table:
{
    ["timeStep"]  = 1.0/1800.0,
    ["subSample"] = 5,
    ["cofr"]      = 0.9,
    ["surfaceFriction"] = 0.5
}
     * @param lua lua_State.
     * @return int lua return code.
     */
    int configure(lua_State * lua);

    /**
     * @brief Returns the current system clock time in lua.
     *
     * @param lua lua_State.
     * @return int return code.
     */
    int timeMillis(lua_State * lua);

    /**
     * @brief Apply a force to an object.
     * @remark Takes the string id, and forces in x and y.
     * @param lua lua_State.
     * @return int return code.
     */
    int lua_applyForce(lua_State * lua);

    /**
     * @brief Lua console.
     * @remark Access to Hop classes is via LuaExtraSpace set by Console::luaStore.
     */
    class Console
    {
    public:

        /**
         * @brief Construct a new Console with a logger.
         *
         * @param l Log outputting Lua's messages.
         */
        Console(Log & l)
        : lastCommandOrProgram(""), lastStatus(false), log(l)
        {
            lua = luaL_newstate();
            luaL_openlibs(lua);
            luaL_requiref(lua,"hop",load_hopLib,1);
            runString("print(\"process running\")");
        }

        ~Console(){ lua_close(lua); }

        /**
         * @brief Attempt to run a Lua script from a file on disc.
         *
         * @param file Lua script location.
         * @return true Error occured.
         * @return false OK.
         */
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

        /**
         * @brief Attempt to run a Lua script from std::string.
         *
         * @param file Lua script.
         * @return true Error occured.
         * @return false OK.
         */
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

        /**
         * @brief Convert Lua's status to a std::string
         *
         * @return std::string Lua status name.
         */
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

        /**
         * @brief Set the LuaExtraSpace holding Hop classes.
         * @remark This gives Console access to Hop.
         * @param ptr LuaExtraSpace.
         */
        void luaStore(LuaExtraSpace * ptr)
        {
            *static_cast<LuaExtraSpace**>(lua_getextraspace(lua)) = ptr;
        }

        /**
         * @brief Run a packed script from Scriptz in LuaExtraSpace.
         *
         * @param name The script name.
         */
        void runScript(std::string name)
        {

            LuaExtraSpace * store = *static_cast<LuaExtraSpace**>(lua_getextraspace(lua));
            Scriptz * scripts = store->scripts;

            std::string body = scripts->get(name);

            if (body == "")
            {
                lua_pushliteral(lua, "script not found");
                lua_error(lua);
            }

            lastCommandOrProgram = name;
            lastStatus =
            (
                luaL_loadstring(lua, body.c_str()) ||
                lua_pcall(lua, 0, LUA_MULTRET, 0)
            );
            handleErrors();
        }

        double getNumber(const char * n)
        {
            LuaNumber num;

            if (num.readGlobal(lua, n))
            {
                return num.n;
            }
            else
            {
                return std::numeric_limits<double>::quiet_NaN();
            }
        }

        std::string getString(const char * n)
        {
            LuaString s;

            if (s.readGlobal(lua, n))
            {
                return s.characters;
            }
            else
            {
                return "";
            }
        }

        bool getBool(const char * n)
        {
            LuaBool s;

            if (s.readGlobal(lua, n))
            {
                return s.bit;
            }
            else
            {
                return false;
            }
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

        bool handleErrors()
        {
            if (lastStatus)
            {
                std::string msg = "Exited with error running "+lastCommandOrProgram+"\n";
                msg += stackTrace;
                ERR(ERRORCODE::LUA_ERROR, msg) >> log;
                return true;
            }
            else
            {
                return false;
            }
        }


        // register lib

        static int load_hopLib(lua_State * lua)
        {
            luaL_Reg hopLib[21] =
            {
                {"loadObject", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_loadObject>},
                {"deleteObject", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_deleteObject>},
                {"getTransform", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_getTransform>},
                {"setTransform", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_setTransform>},
                {"removeFromMeshByTag", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_removeFromMeshByTag>},
                {"meshBoundingBox", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_meshBoundingBox>},
                {"meshBoundingBoxByTag", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_meshBoundingBoxByTag>},
                {"getColour", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_getColour>},
                {"setColour", &dispatchEntityComponentSystem<&EntityComponentSystem::lua_setColour>},
                ///////////////////////////////////////////////////////////////////////////////////////////
                {"maxCollisionPrimitiveSize",&dispatchWorld<&AbstractWorld::lua_worldMaxCollisionPrimitiveSize>},
                ///////////////////////////////////////////////////////////////////////////////////////////
                {"setPhysicsTimeStep",&dispatchsPhysics<&sPhysics::lua_setTimeStep>},
                {"setPhysicsSubSamples",&dispatchsPhysics<&sPhysics::lua_setSubSamples>},
                {"kineticEnergy", &dispatchsPhysics<&sPhysics::lua_kineticEnergy>},
                {"setGravity", &dispatchsPhysics<&sPhysics::lua_setGravity>},
                ///////////////////////////////////////////////////////////////////
                {"setCoefRestitution",&dispatchsCollision<&sCollision::lua_setCOR>},
                {"setSurfaceFriction",&dispatchsCollision<&sCollision::lua_setFriction>},
                ////////////////////////////////////////////////////////////////////
                {"configure", &configure},
                {"timeMillis", &timeMillis},
                {"applyForce", &lua_applyForce},
                ////////////////////////////////////////////////////////////////////
                {"require", &dispatchScriptz<&Scriptz::require>},
                {NULL, NULL}
            };

            luaL_newlib(lua,hopLib);
            return 1;
        }
    };
}

#endif /* CONSOLE_H */
