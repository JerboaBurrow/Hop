/*

    Reads object data given in the following format in Lua

    ```Lua
    object = {

    ["transform"] = {x,y,0.0,1.0},
    ["colour"] = {200/255,200/255,250/255,1.0},
    ["shader"] = "circleObjectShader",
    ["moveable"] = false,
    ["collisionMesh"] =
    {
        {x,y,1.0},
        {y,x,1.0}
    },
    ["name"] = "entity"

    }

    userLib.createObject(object)
    ```

*/

#include <Console/LuaArray.h>
#include <Console/LuaVec.h>
#include <Console/LuaString.h>
#include <Console/LuaBool.h>
#include <Console/LuaTable.h>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    using Hop::System::Physics::CollisionMesh;
    using Hop::System::Physics::CollisionPrimitive;
    using Hop::System::Physics::Rectangle;
    using Hop::Object::Component::cRenderable;
    using Hop::Object::Component::cPhysics;
    using Hop::Object::Component::cCollideable;
    using Hop::LuaArray;

    int EntityComponentSystem::lua_loadObject(lua_State * lua)
    {
        LuaArray<4> colour, transform, util;
        LuaArray<3> meshParameters;

        LuaString shader, name;

        LuaBool isMoveable;

        LuaTable<LuaVec> collisionMesh;

        bool hasColour = false;
        bool hasTransform = false;
        bool isRenderable = false;
        bool isPhysics = false;

        int returnType;

        meshParameters.elements = {CollisionPrimitive::RIGID, 1.0, 1.0};
        isMoveable.bit = true;

        // elements on stack
        int n = lua_gettop(lua);
        
        if (!lua_istable(lua,1))
        {
            lua_pushliteral(lua,"non table argument");
            return lua_error(lua);
        }
        else if (n != 1)
        {
            lua_pushliteral(lua,"requires single argument");
            return lua_error(lua);
        }

        hasColour = colour.read(lua, "colour");
        hasTransform = transform.read(lua, "transform");

        if (hasTransform && hasColour) { isRenderable = true; }

        shader.read(lua, "shader");

        isMoveable.read(lua, "moveable");

        isPhysics = collisionMesh.read(lua, "collisionMesh");

        meshParameters.read(lua, "meshParameters");

        name.read(lua, "name");

        // now create the object

        Id pid;
        
        if (name.characters != "")
        {
            pid = createObject(name.characters);
        }
        else
        {
            pid = createObject();
        }

        double x,y,theta,scale;

        x = transform[0];
        y = transform[1];
        theta = transform[2];
        scale = transform[3];

        addComponent<cTransform>
        (
            pid,
            cTransform(x,y,theta,scale)
        );

        if (isRenderable)
        {
            double r,g,b,a;

            r = colour[0];
            g = colour[1];
            b = colour[2];
            a = colour[3];

            if (shader == "")
            {
                shader.characters = "circleObjectShader";
            }

            float ua = 0.0f;
            float ub = 0.0f;
            float uc = 0.0f;
            float ud = 0.0f;

            if (shader == "lineSegmentObjectShader")
            {
                returnType = lua_getfield(lua, 1, "rectParameters");
                if (returnType == LUA_TTABLE)
                {
                    std::vector<double> param = getNumericLuaTable(lua, 2);

                    if (param.size() != 4)
                    {
                        lua_pushliteral(lua,"rectParameters requires 4 numbers, bottom left x-y, width, height");
                        return lua_error(lua);
                    }

                    ua = param[0];
                    ub = param[1];
                    uc = param[2];
                    ud = param[3];
                
                }
                else if (returnType == LUA_TNONE || returnType == LUA_TNIL)
                {

                }

                lua_pop(lua,1);
                
            }

            addComponent<cRenderable>
            (
                pid,
                cRenderable(shader.characters,r,g,b,a,ua,ub,uc,ud)
            );
        }

        if (isPhysics)
        {
            addComponent<cPhysics>
            (
                pid,
                cPhysics(x,y,theta)
            );

            if (!isMoveable.bit)
            {
                cPhysics & data = getComponent<cPhysics>(pid);
                data.isMoveable = false;
            }

            if (collisionMesh.size() > 0)
            {
                bool goodInput = false;
                std::vector<std::shared_ptr<CollisionPrimitive>> mesh;
                for (unsigned i = 0; i < collisionMesh.size(); i++)
                {
                    if (collisionMesh[i].size() == 3)
                    {
                        mesh.push_back
                        (
                            std::make_shared<CollisionPrimitive>
                            (
                                collisionMesh[i][0],
                                collisionMesh[i][1],
                                collisionMesh[i][2]
                            )
                        );
                        goodInput = true;
                    }
                    else if (collisionMesh[i].size() == 8)
                    {
                        mesh.push_back
                        (
                            std::make_shared<Rectangle>
                            (
                                collisionMesh[i][0],
                                collisionMesh[i][1],
                                collisionMesh[i][2],
                                collisionMesh[i][3],
                                collisionMesh[i][4],
                                collisionMesh[i][5],
                                collisionMesh[i][6],
                                collisionMesh[i][7]
                            )
                        );
                        goodInput = true;
                    }
                }
                if (goodInput)
                {
                    addComponent<cCollideable>
                    (
                        pid,
                        cCollideable(mesh,x,y,theta,scale,meshParameters[0], meshParameters[1], meshParameters[2])
                    );
                }
            }
        }

        lua_pushstring(lua, to_string(pid).c_str());

        return 1;
    }
}