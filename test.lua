s = 0.5*hop.maxCollisionPrimitiveSize()

math.randomseed(os.time())

for i = 1,1 do

    x = 0.905; y = 0.36;-- y = 0.35351;


    object = {

        ["transform"] = {x,y,0.0,s},
        ["colour"] = {200/255,200/255,250/255,1.0},
        -- ["shader"] = "circleObjectShader",
        -- ["shader"] = "lineSegmentObjectShader",
        ["moveable"] = true,
        ["collisionMesh"] =
        {
            -- {0.0,0.0,1.0}
            {-0.25,-0.25,-0.25,0.25,0.25,0.25,0.25,-0.25}
        },
        ["name"] = ""

    }

    hop.loadObject(object)
end