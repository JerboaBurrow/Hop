/*

    Reads object data given in the following format in Lua

    ```Lua
    object = {

    ["transform"] = {x,y,0.0,1.0},
    ["colour"] = {200/255,200/255,250/255,1.0},
    ["texturePath"] = "some/relative/path/to.png",
    ["textureRegion"] = {0, 0, 16, 16},
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
#include <Console/LuaNumber.h>

#include <Object/entityComponentSystem.h>

namespace Hop::Object
{
    using Hop::System::Physics::CollisionMesh;
    using Hop::System::Physics::CollisionPrimitive;
    using Hop::System::Physics::RectanglePrimitive;

    int EntityComponentSystem::lua_deleteObject(lua_State * lua)
    {
        int status = lua_checkArgumentCount(lua, 1, "expected id as argument");
        if (status != LUA_OK) { return status; }
        LuaString sid;

        sid.read(lua, 1);

        Id id(sid.characters);

        remove(id);

        return 0;
    }

    int EntityComponentSystem::lua_loadObject(lua_State * lua)
    {
        int status = lua_checkArgumentCount(lua, 1, "expected table as argument");
        if (status != LUA_OK) { return status; }

        if (!lua_istable(lua,1))
        {
            lua_pushliteral(lua,"non table argument");
            return lua_error(lua);
        }

        LuaArray<4> colour, transform, util, textureRegion;
        LuaArray<3> meshParameters;

        LuaNumber transDrag, rotDrag, bodyMass, bodyInertia, bodyFriction, priority;

        LuaString shader, name, texturePath;

        LuaBool isMoveable, isGhost;

        LuaTable<LuaVec> collisionMesh;

        bool hasColour = false;
        bool hasTransform = false;
        bool hasTexture = false;
        bool hasTextureRegion = false;
        bool isRenderable = false;
        bool isPhysics = false;

        int returnType;

        meshParameters.elements = {CollisionPrimitive::RIGID, 1.0, 1.0};
        isMoveable.bit = true;
        isGhost.bit = false;

        transDrag.n = DEFAULT_TRANSLATIONAL_DRAG;
        rotDrag.n = DEFAULT_ROTATIONAL_DRAG;
        bodyMass.n = DEFAULT_MASS;
        bodyInertia.n = DEFAULT_INTERTIA;
        bodyFriction.n = 0.0;

        priority.n = 0;

        hasColour = colour.readField(lua, "colour");
        hasTransform = transform.readField(lua, "transform");

        if (hasTransform && hasColour) { isRenderable = true; }

        shader.readField(lua, "shader");
        hasTextureRegion = textureRegion.readField(lua, "textureRegion");
        hasTexture = texturePath.readField(lua, "texturePath");
        priority.readField(lua, "renderPriority");

        isMoveable.readField(lua, "moveable");
        isGhost.readField(lua, "ghost");
        isPhysics = collisionMesh.readField(lua, "collisionMesh");
        meshParameters.readField(lua, "meshParameters");

        name.readField(lua, "name");

        transDrag.readField(lua, "translationalDrag");
        rotDrag.readField(lua, "rotationalDrag");
        bodyMass.readField(lua, "mass");
        bodyInertia.readField(lua, "inertia");
        bodyFriction.readField(lua, "bodyFriction");

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
                cRenderable(shader.characters,r,g,b,a,ua,ub,uc,ud,priority.n)
            );

            if (hasTexture)
            {
                jGL::TextureRegion region;
                if (hasTextureRegion)
                {
                    region.tx = textureRegion.elements[0];
                    region.ty = textureRegion.elements[1];
                    region.lx = textureRegion.elements[2];
                    region.ly = textureRegion.elements[3];
                }
                addComponent<cSprite>
                (
                    pid,
                    cSprite(
                        texturePath.characters,
                        region.tx,
                        region.ty,
                        region.lx,
                        region.ty
                    )
                );
            }
        }

        if (isPhysics)
        {
            addComponent<cPhysics>
            (
                pid,
                cPhysics(x,y,theta, transDrag.n, rotDrag.n, bodyInertia.n, bodyMass.n, bodyFriction.n)
            );

            cPhysics & data = getComponent<cPhysics>(pid);
            data.isMoveable = isMoveable.bit;
            data.isGhost = isGhost.bit;

            if (collisionMesh.size() > 0)
            {
                bool goodInput = false;
                std::vector<std::shared_ptr<CollisionPrimitive>> mesh;
                for (unsigned i = 0; i < collisionMesh.size(); i++)
                {
                    if (collisionMesh[i].size() == 3 || collisionMesh[i].size() == 4)
                    {
                        uint64_t tag = collisionMesh[i].size() == 4 ? collisionMesh[i][3] : 0;
                        mesh.push_back
                        (
                            std::make_shared<CollisionPrimitive>
                            (
                                collisionMesh[i][0],
                                collisionMesh[i][1],
                                collisionMesh[i][2],
                                tag,
                                meshParameters[0],
                                meshParameters[1],
                                meshParameters[2]
                            )
                        );
                        goodInput = true;
                    }
                    else if (collisionMesh[i].size() == 8 || collisionMesh[i].size() == 9)
                    {
                        uint64_t tag = collisionMesh[i].size() == 9 ? collisionMesh[i][8] : 0;
                        mesh.push_back
                        (
                            std::make_shared<RectanglePrimitive>
                            (
                                collisionMesh[i][0],
                                collisionMesh[i][1],
                                collisionMesh[i][2],
                                collisionMesh[i][3],
                                collisionMesh[i][4],
                                collisionMesh[i][5],
                                collisionMesh[i][6],
                                collisionMesh[i][7],
                                tag,
                                meshParameters[0]
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
                        cCollideable(mesh,x,y,theta,scale)
                    );
                }
            }
        }

        lua_pushstring(lua, to_string(pid).c_str());

        return 1;
    }
}