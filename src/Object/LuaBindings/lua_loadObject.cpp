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
int EntityComponentSystem::lua_loadObject(lua_State * lua)
{
    std::vector<double> colour, transform, util;
    std::vector< std::vector<double> > collisionMesh;
    std::string shader = "", name = "";
    bool isMoveable = false;
    bool hasColour = false;
    bool hasTransform = false;
    bool isRenderable = false;
    bool isPhysics = false;

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

    // if found puts the elements onto the stack
    // this should be another table (see ex above)
    int returnType = lua_getfield(lua, 1, "colour");

    if (returnType == LUA_TTABLE)
    {
        colour = getNumericLuaTable(lua, 2);

        if (colour.size() != 4)
        {
            lua_pushliteral(lua,"colour requires 4 numbers");
            return lua_error(lua);
        }

        hasColour = true;

    }
    else if (returnType == LUA_TNONE || returnType == LUA_TNIL)
    {

    }

    // don't need the table now
    lua_pop(lua,1);


    returnType = lua_getfield(lua, 1, "transform");

    if (returnType == LUA_TTABLE)
    {
        transform = getNumericLuaTable(lua, 2);

        if (transform.size() != 4)
        {
            lua_pushliteral(lua,"transform requires 4 numbers");
            return lua_error(lua);
        }

        hasTransform = true;
    
    }
    else if (returnType == LUA_TNONE || returnType == LUA_TNIL)
    {

    }

    lua_pop(lua,1);

    // a string should be put onto the stack, if present
    returnType = lua_getfield(lua, 1, "shader");

    if (returnType == LUA_TSTRING)
    {
        shader = lua_tostring(lua, 2);
    }
    else if (returnType == LUA_TNONE || returnType == LUA_TNIL)
    {

    }

    if (hasTransform && hasColour) { isRenderable = true; }

    lua_pop(lua,1);

    returnType = lua_getfield(lua, 1, "moveable");

    if (returnType == LUA_TBOOLEAN)
    {
        isMoveable = lua_toboolean(lua, 2);
        isPhysics = true;
    }
    else if (returnType == LUA_TNONE || returnType == LUA_TNIL)
    {
        lua_pushliteral(lua,"expecting bool got none or nil");
        return lua_error(lua);        
    }

    lua_pop(lua,1);

    returnType = lua_getfield(lua, 1, "collisionMesh");

    if (returnType == LUA_TTABLE)
    {
        collisionMesh = getLuaTableOfNumericLuaTable(lua,2);
        isPhysics = true;
    }
    else if (returnType == LUA_TNONE || returnType == LUA_TNIL)
    {
        lua_pushliteral(lua,"expecting table got none or nil");
        return lua_error(lua);
    }
    else
    {
        lua_pushliteral(lua,"expecting table");
        return lua_error(lua);
    }

    lua_pop(lua,1);

    returnType = lua_getfield(lua, 1, "name");

    if (returnType == LUA_TSTRING)
    {
        name = lua_tostring(lua, 2);
    }
    else if (returnType == LUA_TNONE || returnType == LUA_TNIL)
    {
        lua_pushliteral(lua,"expecting string got none or nil");
        return lua_error(lua);
    }

    lua_pop(lua,1);

    // now create the object

    if (transform.size() == 4)
    {
        Id pid = createObject();

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

            r = 200.0/255.0;
            g = 200.0/255.0;
            b = 1.0;
            a = 1.0;

            if (colour.size() == 4)
            {
                r = colour[0];
                g = colour[1];
                b = colour[2];
                a = colour[3];
            }

            if (shader == "")
            {
                shader = "circleObjectShader";
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
                cRenderable(shader,r,g,b,a,ua,ub,uc,ud)
            );
        }

        if (isPhysics)
        {
            addComponent<cPhysics>
            (
                pid,
                cPhysics(x,y,theta)
            );

            if (!isMoveable)
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
                            std::make_shared<Hop::System::Physics::Rectangle>
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
                        cCollideable(mesh,x,y,theta,scale)
                    );
                }
            }
        }



    }

    return 0;
}